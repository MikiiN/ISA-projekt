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

    int reuse = 1;
    if (setsockopt(fileDescriptor, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        throw SERVER_ERR_SETSOCKOPT;

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
            ldapBind();
            ldapSearch();
            close(sock);
            return;
        }
        else{ // fork error
            throw SERVER_ERR_FORK_FAILED;
        }
    }
    close(fileDescriptor);
}

void LdapServer::ldapBind(){
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
    try{
        sendMessage(buffer);
    }
    catch(int err){
        throw;
    } 
}

void LdapServer::ldapSearch(){
    vector<char> buffer(BUFFER_SIZE);
    int msgSize = read(sock, &buffer[0], buffer.size());
    buffer.resize(msgSize);
    ldap_msg_t decodedMsg;
    ldap_msg_t responseMsg;
    if(ber.decode(buffer, decodedMsg)){
        cout << "Error decode" << endl;
    }
    record_t result;
    try{
        result = db->search(decodedMsg.SearchRequest.filter);
    }
    catch(int err){
        throw;
    }

    responseMsg.OpCode = LDAP_SEARCH_RESULT_ENTRY;
    responseMsg.MsgId = decodedMsg.MsgId;
    responseMsg.SearchResEntry.objName = "uid=" + result.uid;
    search_result_entry_attribute_data_t data;
    data.type = "cn";
    data.value = result.commonName;
    responseMsg.SearchResEntry.attributes.push_back(move(data));
    data.type = "mail";
    data.value = result.email;
    responseMsg.SearchResEntry.attributes.push_back(move(data));
    
    if(ber.encode(buffer, responseMsg)){
        cout << "Error encode resp" << endl;
    }
    try{
        sendMessage(buffer);
    }
    catch(int err){
        throw;
    }
    ldap_msg_t resDoneMsg;
    resDoneMsg.OpCode = LDAP_SEARCH_RESULT_DONE;
    resDoneMsg.MsgId = decodedMsg.MsgId;
    resDoneMsg.SearchResDone.resultCode = SUCCESS;
    resDoneMsg.SearchResDone.matchedDN = "";
    resDoneMsg.SearchResDone.errorMessage = "";
    if(ber.encode(buffer, resDoneMsg)){
        cout << "Error encode resDone" << endl;
    }
    try{
        sendMessage(buffer);
    }
    catch(int err){
        throw;
    }
    msgSize = read(sock, &buffer[0], buffer.size());
    buffer.resize(msgSize);
    if(ber.decode(buffer, decodedMsg)){
        cout << "Error decode" << endl;
    }
    if(decodedMsg.OpCode == LDAP_UNBIND_REQUEST){
        cout << "UNBIND" << endl;
    }
}

void LdapServer::sendMessage(vector<char> msg){
    int msgSize = (int)msg.size();
    if((write(sock, &msg[0], msgSize)) == WRITE_FAILED){
        throw SERVER_ERR_SEND_FAILED;
    }
}