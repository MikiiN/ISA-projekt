/**
 * file: database.cpp
 * author: Michal Žatečka
 * login: xzatec02
*/

#include "headers/database.hpp"

Database::Database(string fileName){
    try{
        loadData(fileName);
    }
    catch(int err){
        throw;
    }
}

record_t Database::search(filter_t &filter){
    bool result;
    record_t empty;
    empty.uid = "";
    while(pos < data.size()){
        try{
            result = matchFilter(data[pos], filter);
        }
        catch(int err){
            if(err == DATABASE_ERR_FILTER_COLUMN){
                // unknown database column
                return empty;
            }
            else{
                throw;
            }
        }
        if(result){ // return matching result
            return data[pos++];
        }
        pos++;
    }
    return empty;
}

void Database::resetDbPosition(){
    pos = 0;
}

void Database::loadData(string fileName){
    ifstream file(fileName);
    if(!file.is_open()){ // check if file is open
        throw DATABASE_ERR_OPEN_FILE_FAILED;
    }
    string line;
    while(getline(file, line)){ // get line from file
        istringstream stringStream(move(line));
        record_t record;
        string value;
        // get common name value from line
        getline(stringStream, value, ';');
        record.commonName = move(value);
        // get uid value from line
        getline(stringStream, value, ';');
        record.uid = move(value);
        // get email value from line
        getline(stringStream, value, ';');
        // remove CR from end of line
        if(isspace(value.back())){
            value.pop_back();
        }
        record.email = move(value);
        data.push_back(move(record));
    }
}

bool Database::matchFilter(record_t &rec, filter_t &filter){
    try{
        switch(filter.type){
            case Fltr_and:
                for(size_t i = 0; i < filter.childs.size(); i++){
                    if(!matchFilter(rec, filter.childs[i])){
                        return false;
                    }
                }
                return true;
            case Fltr_or:
                for(size_t i = 0; i < filter.childs.size(); i++){
                    if(matchFilter(rec, filter.childs[i])){
                        return true;
                    }
                }
                return false;
            case Fltr_not:
                return !matchFilter(rec, filter.childs[0]);
            case Fltr_str_eq:
                return isStrEqual(rec, filter);
            case Fltr_substr:
                return matchSubstr(rec, filter);
            case No_fltr:
                return true;
            default:
                throw INTERNAL_ERR;
        }
    }
    catch(int err){
        throw;
    }
}

bool Database::isStrEqual(record_t &rec, filter_t &filter){
    string column = filter.data[0].column;
    string value = filter.data[0].value; 
    if(!column.compare("uid")){
        return !value.compare(rec.uid);
    }
    else if(!column.compare("cn")){
        return !value.compare(rec.commonName);
    }
    else if(!column.compare("mail")){
        return !value.compare(rec.email);
    }
    else{
        throw DATABASE_ERR_FILTER_COLUMN;
    }
}

bool Database::matchSubstr(record_t &rec, filter_t &filter){
    size_t stringPos = 0;
    bool flag;
    for(size_t i = 0; i < filter.data.size(); i++){
        switch(filter.data[i].type){
            case Substr_start:
                try{
                    flag = matchSubstrBeginning(rec, filter.data[i], stringPos);
                }
                catch(int err){
                    throw;
                }
                break;
            case Substr_contains:
                try{
                    flag = matchSubstrInside(rec, filter.data[i], stringPos);
                }
                catch(int err){
                    throw;
                }
                break;
            case Substr_end:
                try{
                    flag = matchSubstrEnd(rec, filter.data[i], stringPos);
                }
                catch(int err){
                    throw;
                }
                break;
            default:
                throw INTERNAL_ERR;
        }
        if(!flag){
            return false;
        }
    }
    return true;
}

bool Database::matchSubstrBeginning(record_t &rec, filter_string_data_t &data, size_t &strPos){
    string compareValue;
    int substrLength = data.value.size();
    if(strPos != 0){
        throw DATABASE_ERR_FILTER_SUBSTRING;
    }
    if(!data.column.compare("uid")){
        compareValue = rec.uid.substr(0, substrLength);
    }
    else if(!data.column.compare("cn")){
        compareValue = rec.commonName.substr(0, substrLength);
    }
    else if(!data.column.compare("mail")){
        compareValue = rec.email.substr(0, substrLength);
    }
    else{
        throw DATABASE_ERR_FILTER_COLUMN;
    }
    strPos += substrLength;
    return !data.value.compare(compareValue);
}

bool Database::matchSubstrInside(record_t &rec, filter_string_data_t &data, size_t &strPos){
    string compareValue;
    if(!data.column.compare("uid")){
        compareValue = rec.uid.substr(strPos);
    }
    else if(!data.column.compare("cn")){
        compareValue = rec.commonName.substr(strPos);
    }
    else if(!data.column.compare("mail")){
        compareValue = rec.email.substr(strPos);
    }
    else{
        throw DATABASE_ERR_FILTER_COLUMN;
    }
    size_t findPos = compareValue.find(data.value);
    if(findPos == string::npos){
        return false;
    }
    strPos += findPos + data.value.size();
    return true;
}

bool Database::matchSubstrEnd(record_t &rec, filter_string_data_t &data, size_t &strPos){
    string compareValue;
    int substrLength = data.value.size();
    int endPos;
    if(!data.column.compare("uid")){
        endPos = rec.uid.size();
        compareValue = rec.uid.substr(endPos-substrLength, substrLength);
    }
    else if(!data.column.compare("cn")){
        endPos = rec.commonName.size();
        compareValue = rec.commonName.substr(endPos-substrLength, substrLength);
    }
    else if(!data.column.compare("mail")){
        endPos = rec.email.size();
        compareValue = rec.email.substr(endPos-substrLength, substrLength);
    }
    else{
        throw DATABASE_ERR_FILTER_COLUMN;
    }
    if(((int) strPos) > endPos){
        throw DATABASE_ERR_FILTER_SUBSTRING; 
    }
    return !data.value.compare(compareValue);
}