#include "headers/LDAPserver.hpp"

LdapServer::LdapServer(int portNumber, string fileName){
    port = portNumber;
    file = fileName;
    db = new Database(fileName);
}

LdapServer::~LdapServer(){
    delete db;
}

int LdapServer::getParentFD(){
    return fileDescriptor;
}
int LdapServer::getChildFD(){
    return sock;
}

void LdapServer::start(){
    struct sockaddr_in6 server;
    struct sockaddr_in6 client;
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
        if((sock = accept(fileDescriptor, (struct sockaddr *)&client, (socklen_t *)&len)) == -1){
            throw SERVER_ERR_ACCEPT_FAILED;
        }

        pid = fork();
        if(pid > 0){ // parent process
            close(sock);
        }
        else if(pid == 0){ // child process
            close(fileDescriptor);
            LdapBind();
            LdapSearch();
            close(sock);
            return;
        }
        else{ // fork error
            throw SERVER_ERR_FORK_FAILED;
        }
    }
    close(fileDescriptor);
}

void LdapServer::LdapBind(){
    vector<char> buffer(BUFFER_SIZE);
    
    int msgSize = read(sock, &buffer[0], buffer.size());
    buffer.resize(msgSize);
    ldap_msg_t decodedMsg;
    ldap_msg_t responseMsg;
    if(ber.decode(buffer, decodedMsg)){
        cout << "Error decode" << endl;
        responseMsg.BindResponse.resultCode = FAILED;
        responseMsg.BindResponse.errorMessage = "ERROR";
    }
    else{
        responseMsg.BindResponse.resultCode = SUCCESS;
        responseMsg.BindResponse.errorMessage = "";
    }
    responseMsg.OpCode = LDAP_BIND_RESPONSE;
    responseMsg.MsgId = decodedMsg.MsgId;
    responseMsg.BindResponse.matchedDN = "";
    if(ber.encode(buffer, responseMsg)){
        cout << "Error encode" << endl;
        return;
    }
    msgSize = (int)buffer.size();
    if((write(sock, &buffer[0], msgSize)) == WRITE_FAILED){
        cout << "Error write" << endl;
        return;
    }  
}

void LdapServer::LdapSearch(){
    vector<char> buffer(BUFFER_SIZE);
    
    int msgSize = read(sock, &buffer[0], buffer.size());
    buffer.resize(msgSize);
    ldap_msg_t decodedMsg;
    ldap_msg_t responseMsg;
    if(ber.decode(buffer, decodedMsg)){
        cout << "Error decode" << endl;
        responseMsg.BindResponse.resultCode = FAILED;
        responseMsg.BindResponse.errorMessage = "ERROR";
    }
}