#include "headers/database.hpp"

Database::Database(string fileName){
    try{
        loadData(fileName);
    }
    catch(int err){
        throw;
    }
}

void Database::loadData(string fileName){
    ifstream file(fileName);
    if(!file.is_open()){
        throw DATABASE_ERR_OPEN_FILE_FAILED;
    }
    string line;
    while(getline(file, line)){
        istringstream stringStream(move(line));
        record_t record;
        string value;
        getline(stringStream, value, ';');
        record.commonName = move(value);
        getline(stringStream, value, ';');
        record.uid = move(value);
        getline(stringStream, value, ';');
        record.email = move(value);
        data.push_back(move(record));
    }
}