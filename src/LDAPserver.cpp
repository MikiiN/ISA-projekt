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
            ldapCommunication();
            close(sock);
            return;
        }
        else{ // fork error
            throw SERVER_ERR_FORK_FAILED;
        }
    }
    close(fileDescriptor);
}

void LdapServer::ldapCommunication(){
    bool readMsgFlag = true;
    ldap_msg_t decodedMsg;
    try{
        ldapBind();
        while(true){
            ldapSearch(decodedMsg, readMsgFlag);
            readMsgFlag = true;
            if(getUnbind(decodedMsg)){
                break;
            }
            readMsgFlag = false;
        }
    }
    catch(int err){
        throw;
    }
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

void LdapServer::ldapSearch(ldap_msg_t &decodedMsg, bool readMsgFlag){
    vector<char> buffer(BUFFER_SIZE);
    if(readMsgFlag){
        int msgSize = read(sock, &buffer[0], buffer.size());
        buffer.resize(msgSize);
        if(ber.decode(buffer, decodedMsg)){
            cout << "Error decode" << endl;
        }
    }
    
    try{
        sendSearchResults(decodedMsg);
        sendSearchResDone(decodedMsg.MsgId, SUCCESS, "");
    }
    catch(int err){
        throw;
    }
}

void LdapServer::sendMessage(vector<char> msg){
    int msgSize = (int)msg.size();
    if((write(sock, &msg[0], msgSize)) == WRITE_FAILED){
        throw SERVER_ERR_SEND_FAILED;
    }
}

void LdapServer::sendSearchResults(ldap_msg_t &searchRequest){
    int resultCount = 0;
    record_t result;
    
    db->resetDbPosition();
    bool flag = true;
    while(flag){
        try{
            result = db->search(searchRequest.SearchRequest.filter);
        }
        catch(int err){
            throw;
        }
        if(!result.uid.compare("")){
            break;
        }
        resultCount++;
        if(searchRequest.SearchRequest.sizeLimit > 0){
            if(searchRequest.SearchRequest.sizeLimit <= resultCount){
                flag = false;
            }
        }
        try{
            sendResult(result, searchRequest.MsgId);
        }
        catch(int err){
            throw;
        }
    }

}

void LdapServer::sendResult(record_t &record, int msgId){
    ldap_msg_t searchResult;
    searchResult.OpCode = LDAP_SEARCH_RESULT_ENTRY;
    searchResult.MsgId = msgId;
    searchResult.SearchResEntry.objName = "uid=" + record.uid;
    search_result_entry_attribute_data_t data;
    data.type = "cn";
    data.value = record.commonName;
    searchResult.SearchResEntry.attributes.push_back(move(data));
    data.type = "mail";
    data.value = record.email;
    searchResult.SearchResEntry.attributes.push_back(move(data));

    vector<char> buffer;
    if(ber.encode(buffer, searchResult)){
        cout << "Error encode resp" << endl;
        throw SERVER_ERR_ENCODE_DECODE_FAILED;
    }
    try{
        sendMessage(buffer);
    }
    catch(int err){
        throw;
    }
}

void LdapServer::sendSearchResDone(int msgId, int resultCode, string errorMessage){
    ldap_msg_t resDoneMsg;
    vector<char> buffer;
    resDoneMsg.OpCode = LDAP_SEARCH_RESULT_DONE;
    resDoneMsg.MsgId = msgId;
    resDoneMsg.SearchResDone.resultCode = resultCode;
    resDoneMsg.SearchResDone.matchedDN = "";
    resDoneMsg.SearchResDone.errorMessage = errorMessage;
    if(ber.encode(buffer, resDoneMsg)){
        cout << "Error encode resDone" << endl;
    }
    try{
        sendMessage(buffer);
    }
    catch(int err){
        throw;
    }
}

bool LdapServer::getUnbind(ldap_msg_t &decodedMsg){
    vector<char> buffer(BUFFER_SIZE);
    int msgSize = read(sock, &buffer[0], buffer.size());
    buffer.resize(msgSize);
    if(ber.decode(buffer, decodedMsg)){
        cout << "Error decode" << endl;
        throw SERVER_ERR_ENCODE_DECODE_FAILED;
    }
    if(decodedMsg.OpCode == LDAP_UNBIND_REQUEST){
        return true;
    }
    return false;
}