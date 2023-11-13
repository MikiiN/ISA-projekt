#ifndef BER_H
#define BER_H

#include <string>
#include <iostream>
#include <vector>
#include <sstream>

#define SEQUENCE 0x30

#define BOOLEAN 0x01 
#define INTEGER 0x02
#define STRING 0x04
#define ENUMERATED 0x0a

#define FILTER_AND 0xa0
#define FILTER_OR 0xa1
#define FILTER_NOT 0xa2
#define FILTER_EQUALITY_MATCH 0xa3
#define FILTER_SUBSTRING 0xa4

#define SUBSTRING_STARTS_WITH 0x82
#define SUBSTRING_CONTAINS 0x81
#define SUBSTRING_ENDS_WITH 0x80

#define LDAP_BIND_REQUEST 0x60
#define LDAP_BIND_RESPONSE 0x61
#define LDAP_SEARCH_REQUEST 0x63

#define AUTH_SIMPLE 0;

#define OK 0
#define ERR -1

#define MIN_SIZE 2
#define LENGTH_OFFSET 2
#define BYTE 8
#define SHORT_FORM_INDEX_END 1
#define MAX_SHORT_FORM_LENGTH 127
#define SHORT_FORM_HEADER_SIZE 2
#define LONG_FORM_FLAG 128

#define SHORT_OR_LONG_FORM_MASK 128
#define LONG_FORM_MASK 127
#define BYTE_MASK 255

#define DOMAIN_COMPONENT_SEPARATOR ','
#define DOMAIN_COMPONENT_PREFIX "dc="
#define STRING_BEGINNING 0

using namespace std;

typedef struct{
    int version;
    string name;
    int auth;
}bind_request_data_t;

typedef struct{
    vector<string> baseObject;
    int scope;
    int derefAliases;
    int sizeLimit;
    int timeLimit;
    bool typesOnly;
    string filter;
    vector<string> atributes;
}search_request_data_t;

typedef struct{
    int resultCode;
    string matchedDN;
    string errorMessage;
}bind_response_data_t;

typedef struct msg{
    int OpCode;
    int MsgId;
    bind_request_data_t BindRequest;
    bind_response_data_t BindResponse;
    search_request_data_t SearchRequest;
}ldap_msg_t;

class BER{
    public:
        int encode(vector<char> &resultMessage, ldap_msg_t &message);
        int decode(vector<char> message, ldap_msg_t &resultMessage);
    private:
        int position;
        unsigned int getBerLength(vector<char> &message, int &whereLengthEnd);
        int getProtocolData(vector<char> &message, ldap_msg_t &resultMessage);
        int getInt(vector<char> &message);
        string getStr(vector<char> &message);
        int getEnumerated(vector<char> &message);
        bool getBool(vector<char> &message);
        int getBindRequestData(vector<char> &message, ldap_msg_t &resultMessage);
        int getSearchRequestData(vector<char> &message, ldap_msg_t &resultMessage);
        int getSearchRequestBaseObject(vector<char> &message, ldap_msg_t &resultMessage);
        int getSearchRequestFilters(vector<char> &message, ldap_msg_t &resultMessage);
        int getSearchFilterSubstring(vector<char> &message, string &filter);
        int encodeBindResponse(vector<char> &resultMessage, ldap_msg_t &message);
        int addMessageLength(vector<char> &resultMessage, ldap_msg_t &message);
        void addInt(vector<char> &resultMessage, int value);
        void addEnumerated(vector<char> &resultMessage, int value);
        void addString(vector<char> &resultMessage, string value);
        void addLongFormLength(vector<char> &resultMessage, int length);
        void addLongFormInt(vector<char> &resultMessage, int value, int length);
        int calculateBindResponseLength(bind_response_data_t &BindResponse);
        int calculateIntLength(int value);
        int calculateBerHeaderLength(int dataLength);
};

#endif