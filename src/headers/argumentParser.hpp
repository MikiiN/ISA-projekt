/**
 * file: argumentParser.hpp
 * author: Michal Žatečka
 * login: xzatec02
*/

#ifndef ARG_PARSE_H
#define ARG_PARSE_H

#include <string>
#include <vector>

#define MIN_ARG 3
#define MAX_ARG 5
#define ARG_COUNT 2

#define MIN_PORT_NUMBER 0
#define MAX_PORT_NUMBER 65535
#define DEFAULT_PORT_NUMBER 389

#define ARG_START_POS 1

#define ARG_PORT 0
#define ARG_FILE 1
#define ARG_UNKNOWN 2

#define OK 0

#define STEP 2

using namespace std;

class ArgParse{
    private:
        int portNumber;
        string fileName;
        vector<bool> argSetFlags;
        
        /**
         * @brief check number of arguments
         * @param arg number of arguments
         * @return true if number of arguments is correct, false if not
        */
        bool checkArgCount(int argc);

        /**
         * @brief recognize given argument
         * @param arg argument
         * @return argument identification
        */
        int recognizeArg(string arg);

        /**
         * @brief get port number from argument and save it in instance variable
         * @param pNumber port number in string form
         * @return true if pNumber is valid port number, false if not
        */
        bool parsePortNumber(string pNumber);

        /**
         * @brief check if string is a number
         * @param number string with number
         * @return true if is number, false if not
        */
        bool isNumber(string &number);

    public:
        ArgParse();

        /**
         * @brief parse command line arguments
         * @param argc number of command line arguments
         * @param argv array contains command line arguments
        */
        void parse(int argc, char* argv[]);

        /**
         * @brief function to get port number
         * @return number of port
        */
        int getPortNumber();

        /**
         * @brief function to get file name
         * @return file name
        */
        string getFileName();
};

#endif