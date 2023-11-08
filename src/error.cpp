#include "headers/error.hpp"
#include <iostream>

using namespace std;
void printError(int errorCode){
    switch(errorCode){
        case ARG_ERR_MISSING_FILE:
            cerr << "Error: Database file was not specified" << endl;
            break;
        case ARG_ERR_UNKNOWN_ARG:
            cerr << "Error: Unknown argument" << endl;
            break;
        case ARG_ERR_MULTIPLE_ARG:
            cerr << "Error: Multiple occurrence of one argument" << endl; 
            break;
        case ARG_ERR_PORT_IS_NOT_NUMBER:
            cerr << "Error: Port number is not a number" << endl;
            break;
        case ARG_ERR_TOO_MUCH_ARGS:
            cerr << "Error: Wrong number of arguments" << endl;
            break;
        case SERVER_ERR_SOCKET_FAILED:
            cerr << "Error: Socket creation failed" << endl;
            break;
        case SERVER_ERR_BIND_FAILED:
            cerr << "Error: Bind" << endl;
            break;
        case SERVER_ERR_LISTEN:
            cerr << "Error: Listen" << endl;
            break;
        case SERVER_ERR_ACCEPT_FAILED:
            cerr << "Error: Accept" << endl;
            break;
        default:
            cerr << "Error: Unknown error" << endl;
    }
}