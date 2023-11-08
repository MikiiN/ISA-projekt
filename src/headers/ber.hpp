#ifndef BER_H
#define BER_H

#include <string>
#include <iostream>
#include <vector>

#define SEQUENCE 0x30

#define INTEGER 0x02
#define STRING 0x04

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

using namespace std;

typedef struct{
    int version;
    string name;
    int auth;
}bind_request_data_t;


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
        int getBindRequestData(vector<char> &message, ldap_msg_t &resultMessage);
        int encodeBindResponse(vector<char> &resultMessage, ldap_msg_t &message);
        int addMessageLength(vector<char> &resultMessage, ldap_msg_t &message);
        int calculateBindResponseLength(bind_response_data_t &BindResponse);
        int calculateIntLength(int value);
        int calculateBerHeaderLength(int dataLength);
};

#endif