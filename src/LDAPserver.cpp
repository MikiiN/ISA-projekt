#include "headers/LDAPserver.hpp"

LdapServer::LdapServer(int portNumber, string fileName){
    port = portNumber;
    file = fileName;
}

void LdapServer::start(){
    struct sockaddr_in6 server;
    struct sockaddr_in6 client;
    int fileDescriptor;
    int newSock;
    pid_t pid;
    
    if((fileDescriptor = socket(PF_INET6, SOCK_STREAM, 0)) < 0){
		throw SERVER_ERR_SOCKET_FAILED;
	}

    memset(&server, 0, sizeof(server));
    server.sin6_family = AF_INET6;
    server.sin6_addr = in6addr_any;
    server.sin6_port = htons(port);

    if(bind(fileDescriptor, (struct sockaddr *)&server, sizeof(server)) < 0){
        throw SERVER_ERR_BIND_FAILED;
    }

    if(listen(fileDescriptor, QUEUE) != 0){
        throw SERVER_ERR_LISTEN;
    }

    int len = sizeof(client);
    while(true){
        if((newSock = accept(fileDescriptor, (struct sockaddr *)&client, (socklen_t *)&len)) == -1){
            throw SERVER_ERR_ACCEPT_FAILED;
        }

        pid = fork();
        if(pid > 0){ // parent process
            close(newSock);
        }
        else if(pid == 0){ // child process
            close(fileDescriptor);
            LdapBind(newSock);
            // unsigned char buffer[256];
            // int msgSize = read(newSock, buffer, 256);
            // std::cout << "msg size: " << msgSize << std::endl;
            // for(int i = 0; i < msgSize; i++){
            //     std::cout << std::hex << (unsigned int) buffer[i] << " ";
            // }
            close(newSock);
            return;
        }
        else{ // fork error
            throw SERVER_ERR_FORK_FAILED;
        }

        /*
        if(inet_ntop(AF_INET6, &client.sin6_addr, str, sizeof(str))) {
            printf("INFO: New connection:\n");
            printf("INFO: Client address is %s\n", str);
            printf("INFO: Client port is %d\n", ntohs(client.sin6_port));
		}
        */
    }
    close(fileDescriptor);
}

void LdapServer::LdapBind(int sock){
    vector<char> buffer(BUFFER_SIZE);
    
    int msgSize = read(sock, &buffer[0], buffer.size());
    buffer.resize(msgSize);
    ldap_msg_t decodedMsg;
    if(ber.decode(buffer, decodedMsg)){
        cout << "Error decode" << endl;
        return;
    }
    ldap_msg_t responseMsg;
    responseMsg.OpCode = LDAP_BIND_RESPONSE;
    responseMsg.MsgId = decodedMsg.MsgId;
    responseMsg.BindResponse.resultCode = SUCCESS;
    responseMsg.BindResponse.matchedDN = "";
    responseMsg.BindResponse.errorMessage = "";
    if(ber.encode(buffer, responseMsg)){
        cout << "Error encode" << endl;
        return;
    }
    msgSize = (int)buffer.size();
    if((write(sock, &buffer[0], msgSize)) == -1){
        cout << "Error write" << endl;
        return;
    }
    // cout << "op Code: " << decodedMsg.OpCode << endl;
    // cout << "msg id result: " << decodedMsg.MsgId << endl;
    // cout << "version: " << decodedMsg.BindRequest.version << endl;
    // cout << "name: " << decodedMsg.BindRequest.name << endl;
    // // unsigned char bindRespo[15] = {0x30, 0x0c, 0x02, 0x01, 0x01, 0x61, 0x07, 0x0a, 0x01, 0x00, 0x04, 0x00, 0x04, 0x00};
    // // msgSize = write(sock, bindRespo, 15);
    
}