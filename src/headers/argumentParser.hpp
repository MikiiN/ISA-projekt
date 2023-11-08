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
        bool checkArgCount(int argc);
        int recognizeArg(string arg);
        bool parsePortNumber(string pNumber);
        bool isNumber(string &number);

    public:
        ArgParse();
        void parse(int argc, char* argv[]);
        int getPortNumber();
        string getFileName();
};

#endif