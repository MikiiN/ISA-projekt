#include "headers/argumentParser.hpp"
#include "headers/error.hpp"
#include "headers/LDAPserver.hpp"

using namespace std;
int main(int argc, char *argv[]){
    ArgParse arg;
    try{
        arg.parse(argc, argv);
    }
    catch(int err){
        printError(err);
        return err;
    }

    LdapServer server(arg.getPortNumber(), arg.getFileName());
    try{
        server.start();
    }
    catch(int err){
        printError(err);
        return err;
    }

    return 0;
}