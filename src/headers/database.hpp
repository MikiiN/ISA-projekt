/**
 * file: database.hpp
 * author: Michal Žatečka
 * login: xzatec02
*/

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

        /**
         * @brief find first record matching filter (search start on position stored in variable pos)
         * @param filter reference on filter structure 
        */
        record_t search(filter_t &filter);

        /**
         * @brief reset database position on beginning
        */
        void resetDbPosition();
    private:
        size_t pos;
        vector<record_t> data;

        /**
         * @brief load data from file to vector
         * @param fileName name of file
        */
        void loadData(string fileName);

        /**
         * @brief check if record match filter
         * @param rec record 
         * @param filter filter structure
         * @return true if record match filter, false if not
        */
        bool matchFilter(record_t &rec, filter_t &filter);

        /**
         * @brief function for equal match filter
         * @param rec reference on record
         * @param filter reference on filter
         * @return true if strings are equal, false if not
        */
        bool isStrEqual(record_t &rec, filter_t &filter);

        /**
         * @brief function for substrings filter
         * @param rec reference on record
         * @param filter reference on filter
         * @return true if record match filter, false if not
        */
        bool matchSubstr(record_t &rec, filter_t &filter);

        /**
         * @brief function for substring beginning filter
         * @param rec reference on record
         * @param data substring filter data
         * @param strPos record string position
         * @return true if record match filter, false if not
        */
        bool matchSubstrBeginning(record_t &rec, filter_string_data_t &data, size_t &strPos);
        
        /**
         * @brief function for substring filter
         * @param rec reference on record
         * @param data substring filter data
         * @param strPos record string position
         * @return true if record match filter, false if not  
        */
        bool matchSubstrInside(record_t &rec, filter_string_data_t &data, size_t &strPos);

        /**
         * @brief function for substring end filter
         * @param rec reference on record
         * @param data substring filter data
         * @param strPos record string position
         * @return true if record match filter, false if not  
        */
        bool matchSubstrEnd(record_t &rec, filter_string_data_t &data, size_t &strPos);

        /**
         * @brief function for compare case insensitive strings
         * @param s1 first string
         * @param s2 second string
         * @return true if strings are equal, false if not
        */
        bool stringCompare(string s1, string s2);

        /**
         * @brief find substring in string (case insensitive)
         * @param str string
         * @param substr substring
         * @return substring position in string 
        */
        size_t stringFind(string str, string substr);
};

#endif