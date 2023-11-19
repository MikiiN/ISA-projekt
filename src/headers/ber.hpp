/**
 * file: ber.hpp
 * author: Michal Žatečka
 * login: xzatec02
*/

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

/**
 * @brief structure for search request data
*/
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

/**
 * @brief structure for result entry attribute data
*/
typedef struct{
    string type;
    string value;
}search_result_entry_attribute_data_t;

/**
 * @brief structure for result entry data
*/
typedef struct{
    string objName;
    vector<search_result_entry_attribute_data_t> attributes;
}search_result_entry_data_t;

/**
 * @brief structure for bind request data
*/
typedef struct{
    int version;
    string name;
    int auth;
}bind_request_data_t;

/**
 * @brief structure for bind response data
*/
typedef struct{
    int resultCode;
    string matchedDN;
    string errorMessage;
}bind_response_search_done_data_t;

/**
 * @brief structure for LDAP message
*/
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
        /**
         * @brief function for encoding message
         * @param resultMessage vector for encoded message
         * @param message message data for encoding
         * @return 0 if success, 0< if error
        */
        int encode(vector<char> &resultMessage, ldap_msg_t &message);

        /**
         * @brief function for decoding message received from client
         * @param message message received from client
         * @param resultMessage structure for decoded message data
         * @return 0 if success, 0< if error
        */
        int decode(vector<char> message, ldap_msg_t &resultMessage);
    private:
        /**
         * @brief position in message
        */
        int position;

        /**
         * @brief get length from BER header
         * @param message message from client
         * @param whereLengthEnd reference for return index where length ended
         * @return data length
        */
        unsigned int decodeBerLength(vector<char> &message, int &whereLengthEnd);
        
        /**
         * @brief skip BER tag and length in message
         * @param message message from client
        */
        void skipBerTagLength(vector<char> &message);

        /**
         * @brief function for decoding protocol data
         * @param message message from client
         * @param resultMessage reference on structure for decoded data
         * @return error code
        */
        int decodeProtocolData(vector<char> &message, ldap_msg_t &resultMessage);

        /**
         * @brief get integer from message
         * @param message message from client
         * @return decoded int
        */
        int decodeInt(vector<char> &message);

        /**
         * @brief get string from message
         * @param message message from client
         * @return decoded string
        */
        string decodeStr(vector<char> &message);

        /**
         * @brief get enumerated value
         * @param message message from client
         * @return decoded enumerated value
        */
        int decodeEnumerated(vector<char> &message);

        /**
         * @brief get bool from message
         * @param message message from client
         * @return decoded bool value
        */
        bool decodeBool(vector<char> &message);

        /**
         * @brief get bind request data from message
         * @param message message from client
         * @param resultMessage reference on structure for LDAP message data
         * @return error code
        */
        int decodeBindRequestData(vector<char> &message, ldap_msg_t &resultMessage);

        /**
         * @brief get search request data from message
         * @param message message from client
         * @param resultMessage reference on structure for LDAP message data
         * @return error code
        */
        int decodeSearchRequestData(vector<char> &message, ldap_msg_t &resultMessage);

        /**
         * @brief get unbind request data from message
         * @param message message from client
         * @return error code
        */
        int decodeUnbindRequestData(vector<char> &message);

        /**
         * @brief get search request base object data
         * @param message message from client
         * @param resultMessage reference on structure for LDAP message data
         * @return error code
        */
        int decodeSearchRequestBaseObject(vector<char> &message, ldap_msg_t &resultMessage);

        /**
         * @brief get search request filter data from message
         * @param message message from client
         * @param resultMessage reference on structure for LDAP message data
         * @return error code
        */
        int decodeSearchRequestFilters(vector<char> &message, ldap_msg_t &resultMessage);

        /**
         * @brief function to choose filter type
         * @param message message from client
         * @return filter data 
        */
        filter_t decodeSearchRequestFiltersSwitch(vector<char> &message);

        /**
         * @brief get substring filter data
         * @param message message from client
         * @return filter data
        */
        filter_t decodeSearchFilterSubstring(vector<char> &message);

        /**
         * @brief get string match filter data
         * @param message message from client
         * @return filter data
        */
        filter_t decodeSearchFilterStringMatch(vector<char> &message);

        /**
         * @brief get and/or/not filter data
         * @param message message from client
         * @param type filter type
         * @return filter data
        */
        filter_t decodeSearchFilterAndOrNot(vector<char> &message, filter_type type);

        /**
         * @brief encode bind response data
         * @param resultMessage vector for encoded message
         * @param message LDAP message data
         * @return error code
        */
        int encodeBindResponse(vector<char> &resultMessage, ldap_msg_t &message);
        
        /**
         * @brief encode search result entry data
         * @param resultMessage vector for encoded message
         * @param message LDAP message data
         * @return error code
        */
        int encodeSearchResEntry(vector<char> &resultMessage, ldap_msg_t &message);
        
        /**
         * @brief encode search result done data
         * @param resultMessage vector for encoded message
         * @param message LDAP message data
         * @return error code
        */
        int encodeSearchResDone(vector<char> &resultMessage, ldap_msg_t &message);
        
        /**
         * @brief encode whole message length 
         * @param resultMessage vector for encoded message
         * @param message LDAP message data
         * @return error code
        */
        int addMessageLength(vector<char> &resultMessage, ldap_msg_t &message);

        /**
         * @brief encode search results attributes
         * @param resultMessage vector for encoded message
         * @param attributes result data 
        */
        void encodeSearchResEntryAttributes(vector<char> &resultMessage, vector<search_result_entry_attribute_data_t> attributes);
        
        /**
         * @brief encode integer
         * @param resultMessage vector for encoded message
         * @param value integer value
        */
        void encodeInt(vector<char> &resultMessage, int value);
        
        /**
         * @brief encode enumerated
         * @param resultMessage vector for encoded message
         * @param value enumerated value
        */
        void encodeEnumerated(vector<char> &resultMessage, int value);
        
        /**
         * @brief encode string
         * @param resultMessage vector for encoded message
         * @param value string value
        */
        void encodeString(vector<char> &resultMessage, string value);

        /**
         * @brief encode length
         * @param resultMessage vector for encoded message
         * @param length length 
        */
        void addLength(vector<char> &resultMessage, int length);
        
        /**
         * @brief encode long form length
         * @param resultMessage vector for encoded message
         * @param length length
        */
        void addLongFormLength(vector<char> &resultMessage, int length);
        
        /**
         * @brief encode long form integer
         * @param resultMessage vector for encoded message
         * @param value integer value
         * @param length integer length
        */
        void addLongFormInt(vector<char> &resultMessage, int value, int length);
        
        /**
         * @brief calculate bind response or search result done length
         * @param data bind response or search result done data
         * @return data length 
        */
        int calculateBindResponseResDoneLength(bind_response_search_done_data_t &data);
        
        /**
         * @brief calculate search result entry length
         * @param searchResEntry search result entry data
         * @return data length 
        */
        int calculateSearchResEntryLength(search_result_entry_data_t &searchResEntry);
        
        /**
         * @brief calculate search result entry attributes length
         * @param attributes vector of attribute
         * @return attributes length 
        */
        int calculateResEntryAttribsLength(vector<search_result_entry_attribute_data_t> attributes);
        
        /**
         * @brief calculate search result entry attribute length
         * @param attribute attribute data
         * @return attribute length 
        */
        int calculateResEntryAttribLength(search_result_entry_attribute_data_t attribute);

        /**
         * @brief calculate integer length
         * @param value integer value
         * @return length
        */
        int calculateIntLength(int value);

        /**
         * @brief calculate BER tag and length length
         * @param dataLength length of data
         * @return BER tag and length length
        */
        int calculateBerHeaderLength(int dataLength);
};

#endif