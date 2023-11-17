#ifndef DATABASE_H
#define DATABASE_H

#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include "error.hpp"
#include "filter.hpp"

using namespace std;

typedef struct{
    string commonName;
    string uid;
    string email;
}record_t;

class Database{
    public:
        Database(string fileName);
        record_t search(filter_t &filter);
        void resetDbPosition();
    private:
        size_t pos;
        vector<record_t> data;
        void loadData(string fileName);
        bool matchFilter(record_t &rec, filter_t &filter);
        bool isStrEqual(record_t &rec, filter_t &filter);
        bool matchSubstr(record_t &rec, filter_t &filter);
        bool matchSubstrBeginning(record_t &rec, filter_string_data_t &data, size_t &strPos);
        bool matchSubstrInside(record_t &rec, filter_string_data_t &data, size_t &strPos);
        bool matchSubstrEnd(record_t &rec, filter_string_data_t &data, size_t &strPos);
};


#endif