/**
 * file: error.cpp
 * author: Michal Žatečka
 * login: xzatec02
*/

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
            cerr << "Error: Bind failed" << endl;
            break;
        case SERVER_ERR_LISTEN:
            cerr << "Error: Listen failed" << endl;
            break;
        case SERVER_ERR_ACCEPT_FAILED:
            cerr << "Error: Accept failed" << endl;
            break;
        case SERVER_ERR_FORK_FAILED:
            cerr << "Error: Fork failed" << endl;
            break;
        case SERVER_ERR_SETSOCKOPT:
            cerr << "Error: Setting socket options failed" << endl;
            break;
        case SERVER_ERR_SEND_FAILED:
            cerr << "Error: Sending message failed" << endl;
            break;
        case SERVER_ERR_ENCODE_DECODE_FAILED:
            cerr << "Error: Encoding or decoding message failed" << endl;
            break;
        case DATABASE_ERR_OPEN_FILE_FAILED:
            cerr << "Error: Can't open the file" << endl;
            break;
        case DATABASE_ERR_FILTER_SUBSTRING:
            cerr << "Error: Substring Filter" << endl;
            break;
        case DATABASE_ERR_FILTER_COLUMN:
            cerr << "Error: Unknown database column" << endl;
            break;
        case INTERNAL_ERR:
            cerr << "Error: Internal error" << endl;
            break;
        default:
            cerr << "Error: Unknown error" << endl;
    }
}