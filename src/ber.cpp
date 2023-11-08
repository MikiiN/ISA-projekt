#include "headers/ber.hpp"

int BER::decode(vector<char> message, ldap_msg_t &resultMessage){
    // clear decode message struct
    resultMessage = {};
    
    position = 0;
    if(message.size() < MIN_SIZE){
        return ERR;
    }
    if(message[0] != SEQUENCE){
        return ERR;
    }
    int whereLengthEnd;
    if(getBerLength(message, whereLengthEnd) != (message.size() - LENGTH_OFFSET)){
        return ERR;
    }
    position += whereLengthEnd+1; 
    resultMessage.MsgId = getInt(message);
    if(resultMessage.MsgId == ERR){
        return ERR;
    }
    if(getProtocolData(message, resultMessage)){
        return ERR;
    }
    return OK;
}

unsigned int BER::getBerLength(vector<char> &message, int &whereLengthEnd){
    if(message[position + 1] & SHORT_OR_LONG_FORM_MASK){
        // long form
        unsigned int lengthOfLength = (unsigned int)(message[position + 1] & LONG_FORM_MASK);
        unsigned int length = 0;
        whereLengthEnd = lengthOfLength+1;
        unsigned int msgIndex;
        for(unsigned int i = 0; i < lengthOfLength; i++){
            msgIndex = (lengthOfLength - 1) - i + LENGTH_OFFSET;
            length += (unsigned int) (message[position+msgIndex] << (sizeof(char) * BYTE * i));
        }
        return length;
    }
    else{
        /// short form
        whereLengthEnd = SHORT_FORM_INDEX_END;
        return (unsigned int) message[position+1];
    }
}

int BER::getInt(vector<char> &message){
    if(message[position] != INTEGER){
        return ERR;
    }
    int whereLengthEnd;
    unsigned int length = getBerLength(message, whereLengthEnd);
    int result = 0;
    int index;
    for(unsigned int i = 0; i < length; i++){
        index = whereLengthEnd + (length - i);
        result += (int) (message[position + index] << (sizeof(char) * BYTE * i));
    }
    int intLen = length + LENGTH_OFFSET;
    // skip int in message
    position += intLen;
    return result;
}

string BER::getStr(vector<char> &message){
    if(message[position] != STRING){
        throw ERR;
    }
    int whereLengthEnd;
    unsigned int length = getBerLength(message, whereLengthEnd);
    if(length == 0){
        position += LENGTH_OFFSET;
        return "";
    }
    int skip = position+whereLengthEnd+1;
    string result(message.begin()+skip, message.begin()+skip+length);
    position += whereLengthEnd+1;
    return result;
}

int BER::getProtocolData(vector<char> &message, ldap_msg_t &resultMessage){
    switch(message[position]){
        case LDAP_BIND_REQUEST:
            resultMessage.OpCode = LDAP_BIND_REQUEST;
            if(getBindRequestData(message, resultMessage) == ERR)
                return ERR;
            break;
        case LDAP_SEARCH_REQUEST:
            break;
        default:
            return ERR;
            break;
    }
    return OK;
}

int BER::getBindRequestData(vector<char> &message, ldap_msg_t &resultMessage){
    int whereLengthEnd;
    getBerLength(message, whereLengthEnd);
    position += whereLengthEnd+1;
    resultMessage.BindRequest.version = getInt(message);
    resultMessage.BindRequest.name = getStr(message);
    return OK;
}

int BER::encode(vector<char> &resultMessage, ldap_msg_t &message){
    position = 0;
    resultMessage.clear();
    switch(message.OpCode){
        case LDAP_BIND_RESPONSE:
            return encodeBindResponse(resultMessage, message);        
            break;
        default:
            return ERR;
    }
}

int BER::encodeBindResponse(vector<char> &resultMessage, ldap_msg_t &message){
    resultMessage.push_back(SEQUENCE);
    if(addMessageLength(resultMessage, message)){
        return ERR;
    }
    return OK;
}

int BER::addMessageLength(vector<char> &resultMessage, ldap_msg_t &message){
    int length = calculateIntLength(message.MsgId) + SHORT_FORM_HEADER_SIZE;
    int tmpLength;
    switch(message.OpCode){
        case LDAP_BIND_RESPONSE:
            tmpLength = calculateBindResponseLength(message.BindResponse);
            length += calculateBerHeaderLength(tmpLength) + tmpLength;
            break;
        default:
            return ERR;
    }
    if(length > MAX_SHORT_FORM_LENGTH){
        int lengthOfLength = calculateIntLength(length);
        resultMessage.push_back(lengthOfLength + LONG_FORM_FLAG);
        int value;
        for(int i = lengthOfLength-1; i >= 0; i--){
            value = length & (BYTE_MASK << (i * BYTE));
            resultMessage.push_back((char) value);
        }
    }
    else{
        resultMessage.push_back(length);
    }
    return OK;
}

int BER::calculateBindResponseLength(bind_response_data_t &BindResponse){
    int resultCodeLength = calculateIntLength(BindResponse.resultCode); 
    int length = calculateBerHeaderLength(resultCodeLength) + resultCodeLength;
    length += calculateBerHeaderLength((int)BindResponse.matchedDN.size()) + BindResponse.matchedDN.size();
    length += calculateBerHeaderLength((int)BindResponse.errorMessage.size()) + BindResponse.errorMessage.size();
    return length;
}

int BER::calculateIntLength(int value){
    int l;
    for(l = 1; (value >> l*BYTE) != 0; l++);
    return l;
}

int BER::calculateBerHeaderLength(int dataLength){
    int length;
    if(dataLength > MAX_SHORT_FORM_LENGTH){
        length = SHORT_FORM_HEADER_SIZE + calculateIntLength(dataLength);
    }
    else{
        length = SHORT_FORM_HEADER_SIZE;
    }
    return length;
}