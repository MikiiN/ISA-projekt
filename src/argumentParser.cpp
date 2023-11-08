#include "headers/argumentParser.hpp"
#include "headers/error.hpp"

int ArgParse::getPortNumber(){
    return portNumber;
}

string ArgParse::getFileName(){
    return fileName;
}

ArgParse::ArgParse(){
    portNumber = DEFAULT_PORT_NUMBER;
    fileName.clear();
    argSetFlags.resize(ARG_COUNT, false);
}

void ArgParse::parse(int argc, char* argv[]){
    if(!checkArgCount(argc)){
        throw ARG_ERR_TOO_MUCH_ARGS;
    }

    for(int i = ARG_START_POS; i < argc; i += STEP){
        switch(recognizeArg(argv[i])){
            case ARG_PORT:
                if(argSetFlags[ARG_PORT])
                    throw ARG_ERR_MULTIPLE_ARG;
                if(!parsePortNumber(argv[i+1]))
                    throw ARG_ERR_PORT_IS_NOT_NUMBER;
                argSetFlags[ARG_PORT] = true;
                break;
            case ARG_FILE:
                if(argSetFlags[ARG_FILE])
                    throw ARG_ERR_MULTIPLE_ARG;
                fileName = argv[i+1];
                argSetFlags[ARG_FILE] = true;
                break;
            default:
                throw ARG_ERR_UNKNOWN_ARG;
        }
    }
    if(!argSetFlags[ARG_FILE])
        throw ARG_ERR_MISSING_FILE;
}

bool ArgParse::checkArgCount(int argc){
    if(argc < MIN_ARG || argc > MAX_ARG)
        return false;
    if(argc % 2 == 0) // only odd numbers are valid
        return false;
    return true;
}

int ArgParse::recognizeArg(string arg){
    if(!arg.compare("-p") || !arg.compare("--port")){
        return ARG_PORT;
    }
    if(!arg.compare("-f") || !arg.compare("--file")){
        return ARG_FILE;
    }
    return ARG_UNKNOWN;

}

bool ArgParse::parsePortNumber(string pNumber){
    if(!isNumber(pNumber))
        return false;
    int result = stoi(pNumber);
    if(result < MIN_PORT_NUMBER || result > MAX_PORT_NUMBER)
        return false;
    portNumber = result;
    return true;
}

bool ArgParse::isNumber(string &number){
    string::iterator iter = number.begin();
    while(iter != number.end()){
        if(!isdigit(*iter))
            return false;
        iter++;
    }
    return true;
}