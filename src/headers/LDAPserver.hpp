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
#define FAILED 1

#define WRITE_FAILED -1


using namespace std;

class LdapServer{
    public:
        LdapServer(int portNumber, string fileName);
        ~LdapServer();
        void start();
        int getParentFD();
        int getChildFD();
    private:
        int port;
        string file;
        BER ber;
        Database *db;
        int fileDescriptor;
        int sock;
        void LdapBind();
        void LdapSearch();
};

#endif