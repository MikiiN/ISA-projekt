#ifndef BER_H
#define BER_H

#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include "filter.hpp"

#define SEQUENCE 0x30
#define SET 0x31

#define BOOLEAN 0x01 
#define INTEGER 0x02
#define STRING 0x04
#define ENUMERATED 0x0a

#define LDAP_BIND_REQUEST 0x60
#define LDAP_BIND_RESPONSE 0x61
#define LDAP_UNBIND_REQUEST 0x42
#define LDAP_SEARCH_REQUEST 0x63
#define LDAP_SEARCH_RESULT_ENTRY 0x64
#define LDAP_SEARCH_RESULT_DONE 0x65

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
    vector<string> baseObject;
    int scope;
    int derefAliases;
    int sizeLimit;
    int timeLimit;
    bool typesOnly;
    filter_t filter;
    vector<string> atributes;
}search_request_data_t;

typedef struct{
    string type;
    string value;
}search_result_entry_attribute_data_t;

typedef struct{
    string objName;
    vector<search_result_entry_attribute_data_t> attributes;
}search_result_entry_data_t;

typedef struct{
    int version;
    string name;
    int auth;
}bind_request_data_t;

typedef struct{
    int resultCode;
    string matchedDN;
    string errorMessage;
}bind_response_search_done_data_t;

typedef struct msg{
    int OpCode;
    int MsgId;
    bind_request_data_t BindRequest;
    bind_response_search_done_data_t BindResponse;
    search_request_data_t SearchRequest;
    search_result_entry_data_t SearchResEntry;
    bind_response_search_done_data_t SearchResDone;
}ldap_msg_t;

class BER{
    public:
        int encode(vector<char> &resultMessage, ldap_msg_t &message);
        int decode(vector<char> message, ldap_msg_t &resultMessage);
    private:
        int position;
        unsigned int getBerLength(vector<char> &message, int &whereLengthEnd);
        void skipBerTagLength(vector<char> &message);
        int getProtocolData(vector<char> &message, ldap_msg_t &resultMessage);
        int getInt(vector<char> &message);
        string getStr(vector<char> &message);
        int getEnumerated(vector<char> &message);
        bool getBool(vector<char> &message);
        int decodeBindRequestData(vector<char> &message, ldap_msg_t &resultMessage);
        int decodeSearchRequestData(vector<char> &message, ldap_msg_t &resultMessage);
        int decodeUnbindRequestData(vector<char> &message);
        int decodeSearchRequestBaseObject(vector<char> &message, ldap_msg_t &resultMessage);
        int decodeSearchRequestFilters(vector<char> &message, ldap_msg_t &resultMessage);
        filter_t decodeSearchRequestFiltersSwitch(vector<char> &message);
        filter_t decodeSearchFilterSubstring(vector<char> &message);
        filter_t decodeSearchFilterStringMatch(vector<char> &message);
        filter_t decodeSearchFilterAndOrNot(vector<char> &message, filter_type type);
        int encodeBindResponse(vector<char> &resultMessage, ldap_msg_t &message);
        int encodeSearchResEntry(vector<char> &resultMessage, ldap_msg_t &message);
        int encodeSearchResDone(vector<char> &resultMessage, ldap_msg_t &message);
        int addMessageLength(vector<char> &resultMessage, ldap_msg_t &message);
        void encodeSearchResEntryAttributes(vector<char> &resultMessage, vector<search_result_entry_attribute_data_t> attributes);
        void encodeInt(vector<char> &resultMessage, int value);
        void encodeEnumerated(vector<char> &resultMessage, int value);
        void encodeString(vector<char> &resultMessage, string value);
        void addLength(vector<char> &resultMessage, int length);
        void addLongFormLength(vector<char> &resultMessage, int length);
        void addLongFormInt(vector<char> &resultMessage, int value, int length);
        int calculateBindResponseResDoneLength(bind_response_search_done_data_t &data);
        int calculateSearchResEntryLength(search_result_entry_data_t &searchResEntry);
        int calculateResEntryAttribsLength(vector<search_result_entry_attribute_data_t> attributes);
        int calculateResEntryAttribLength(search_result_entry_attribute_data_t attribute);
        int calculateIntLength(int value);
        int calculateBerHeaderLength(int dataLength);
};

#endif