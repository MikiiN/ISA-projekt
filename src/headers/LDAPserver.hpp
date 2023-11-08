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

#define QUEUE 1
#define BUFFER_SIZE 256

using namespace std;
class LdapServer{
    public:
        LdapServer(int portNumber, string fileName);
        void start();
    private:
        int port;
        string file;
        BER ber;
        void LdapBind(int sock);
};

#endif