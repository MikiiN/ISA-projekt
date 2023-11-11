#include "headers/argumentParser.hpp"
#include "headers/error.hpp"
#include "headers/LDAPserver.hpp"

using namespace std;

LdapServer *server;
ArgParse *arg;

void sigHandler(int sig){
    (void) sig;
    close(server->getParentFD());
    close(server->getChildFD());
    delete server;
    delete arg;
    exit(0);
}

int main(int argc, char *argv[]){
    arg = new ArgParse();
    try{
        arg->parse(argc, argv);
    }
    catch(int err){
        printError(err);
        return err;
    }

    server = new LdapServer(arg->getPortNumber(), arg->getFileName());
    signal(SIGINT, sigHandler);
    try{
        server->start();
    }
    catch(int err){
        printError(err);
        delete server;
        delete arg;
        return err;
    }

    delete server;
    delete arg;
    return 0;
}