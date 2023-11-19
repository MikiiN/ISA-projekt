/**
 * file: LDAPserver.hpp
 * author: Michal Žatečka
 * login: xzatec02
*/

#ifndef LDAP_SERVER_H
#define LDAP_SERVER_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include "error.hpp"
#include "ber.hpp"
#include "database.hpp"

#define QUEUE 1
#define BUFFER_SIZE 512

#define SUCCESS 0
#define OPERATION_ERROR 1
#define PROTOCOL_ERROR 2
#define AUTH_METHOD_NOT_SUPPORTED 7

#define WRITE_FAILED -1


using namespace std;

class LdapServer{
    public:
        LdapServer(int portNumber, string fileName);
        ~LdapServer();
        
        /**
         * @brief start LDAP server
        */
        void start();

        /**
         * @brief get parent socket file descriptor
         * @return parent socket file descriptor
        */
        int getParentFD();

        /**
         * @brief get child socket file descriptor
         * @return child socked file descriptor
        */
        int getChildFD();
    private:
        int port;
        string file;
        BER ber;
        Database *db;
        int fileDescriptor;
        int sock;

        /**
         * @brief function controlling LDAP communication
        */
        void ldapCommunication();

        /**
         * @brief function for LDAP bind
        */
        void ldapBind();

        /**
         * @brief function for LDAP search operation
         * @param decodedMsg reference on decoded LDAP message
         * @param redMsgFlag if function have to read message
        */
        void ldapSearch(ldap_msg_t &decodedMsg, bool readMsgFlag);

        /**
         * @brief function for sending message
         * @param msg reference on message buffer
        */
        void sendMessage(vector<char> &msg);

        /**
         * @brief function for sending search results
         * @param searchRequest reference on decoded search request message
        */
        void sendSearchResults(ldap_msg_t &searchRequest);

        /**
         * @brief send single search result
         * @param record reference on record from database
         * @param msgId search request message ID 
        */
        void sendResult(record_t &record, int msgId);

        /**
         * @brief send search result done message
         * @param msgId search request message ID
         * @param resultCode search result done message result code
         * @param errorMessage search result done message error string
        */
        void sendSearchResDone(int msgId, int resultCode, string errorMessage);

        /**
         * @brief function to check if server received unbind request
         * @param decodedMsg reference on structure for decoded LDAP message
         * @return true if server received unbind request, false if not
        */
        bool getUnbind(ldap_msg_t &decodedMsg);
};

#endif