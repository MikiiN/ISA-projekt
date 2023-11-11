#ifndef DATABASE_H
#define DATABASE_H

#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include "error.hpp"

using namespace std;

typedef struct{
    string commonName;
    string uid;
    string email;
}record_t;

class Database{
    public:
        Database(string fileName);
    private:
        vector<record_t> data;
        void loadData(string fileName);
};


#endif