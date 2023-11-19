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
    if(decodeBerLength(message, whereLengthEnd) != (message.size() - whereLengthEnd)){
        return ERR;
    }
    position += whereLengthEnd; // skip tag and length
    resultMessage.MsgId = decodeInt(message);
    if(resultMessage.MsgId == ERR){
        return ERR;
    }
    if(decodeProtocolData(message, resultMessage)){
        return ERR;
    }
    return OK;
}

unsigned int BER::decodeBerLength(vector<char> &message, int &whereLengthEnd){
    if(message[position + 1] & SHORT_OR_LONG_FORM_MASK){
        // long form
        unsigned int lengthOfLength = (unsigned int)(message[position + 1] & LONG_FORM_MASK);
        unsigned int length = 0;
        whereLengthEnd = lengthOfLength + SHORT_FORM_HEADER_SIZE;
        unsigned int msgIndex;
        for(unsigned int i = 0; i < lengthOfLength; i++){
            msgIndex = (lengthOfLength - 1) - i + LENGTH_OFFSET;
            length += (unsigned int) (message[position+msgIndex] << (sizeof(char) * BYTE * i));
        }
        return length;
    }
    else{
        /// short form
        whereLengthEnd = SHORT_FORM_HEADER_SIZE;
        return (unsigned int) message[position+1];
    }
}

void BER::skipBerTagLength(vector<char> &message){
    int whereLengthEnd;
    decodeBerLength(message, whereLengthEnd);
    position += whereLengthEnd;
}

int BER::decodeInt(vector<char> &message){
    if(message[position] != INTEGER){
        return ERR;
    }
    int whereLengthEnd;
    unsigned int length = decodeBerLength(message, whereLengthEnd);
    int result = 0;
    int index;
    for(unsigned int i = 1; i <= length; i++){
        index = whereLengthEnd + (length - i);
        result += (int) (message[position + index] << (sizeof(char) * BYTE * (i-1)));
    }
    int intLen = length + whereLengthEnd;
    // skip int in message
    position += intLen;
    return result;
}

string BER::decodeStr(vector<char> &message){
    if(message[position] != STRING){
        throw ERR;
    }
    int whereLengthEnd;
    unsigned int length = decodeBerLength(message, whereLengthEnd);
    if(length == 0){
        position += LENGTH_OFFSET;
        return "";
    }
    int skip = position+whereLengthEnd;
    string result(message.begin()+skip, message.begin()+skip+length);
    // skip string
    position += whereLengthEnd+length;
    return result;
}

int BER::decodeEnumerated(vector<char> &message){
    if(message[position] != ENUMERATED){
        return ERR;
    }
    int whereLengthEnd;
    unsigned int length = decodeBerLength(message, whereLengthEnd);
    int result = 0;
    int index;
    for(unsigned int i = 1; i <= length; i++){
        index = whereLengthEnd + (length - i);
        result += (int) (message[position + index] << (sizeof(char) * BYTE * (i-1)));
    }
    int enumLen = length + whereLengthEnd;
    // skip enum in message
    position += enumLen;
    return result;
}

bool BER::decodeBool(vector<char> &message){
    if(message[position] != BOOLEAN){
        throw ERR;
    }
    position += LENGTH_OFFSET;
    bool result = message[position];
    position++;
    return result;
}

int BER::decodeProtocolData(vector<char> &message, ldap_msg_t &resultMessage){
    switch(message[position]){
        case LDAP_BIND_REQUEST:
            resultMessage.OpCode = LDAP_BIND_REQUEST;
            if(decodeBindRequestData(message, resultMessage) == ERR){
                return ERR;
            }
            break;
        case LDAP_SEARCH_REQUEST:
            resultMessage.OpCode = LDAP_SEARCH_REQUEST;
            if(decodeSearchRequestData(message, resultMessage) == ERR){
                return ERR;
            }
            break;
        case LDAP_UNBIND_REQUEST:
            resultMessage.OpCode = LDAP_UNBIND_REQUEST;
            if(decodeUnbindRequestData(message) == ERR){
                return ERR;
            }
            break;
        default:
            return ERR;
            break;
    }
    return OK;
}

int BER::decodeBindRequestData(vector<char> &message, ldap_msg_t &resultMessage){
    int whereLengthEnd;
    decodeBerLength(message, whereLengthEnd);
    position += whereLengthEnd;
    try{
        resultMessage.BindRequest.version = decodeInt(message);
        resultMessage.BindRequest.name = decodeStr(message);
    }
    catch(int err){
        return err;
    }
    return OK;
}

int BER::decodeSearchRequestData(vector<char> &message, ldap_msg_t &resultMessage){
    int whereLengthEnd;
    decodeBerLength(message, whereLengthEnd);
    position += whereLengthEnd;
    if(decodeSearchRequestBaseObject(message, resultMessage)){
        return ERR;
    }
    try{
        resultMessage.SearchRequest.scope = decodeEnumerated(message);
        resultMessage.SearchRequest.derefAliases = decodeEnumerated(message);
        resultMessage.SearchRequest.sizeLimit = decodeInt(message);
        resultMessage.SearchRequest.timeLimit = decodeInt(message);
        resultMessage.SearchRequest.typesOnly = decodeBool(message);
    }
    catch(int err){
        return err;
    }
    if(decodeSearchRequestFilters(message, resultMessage)){
        return ERR;
    }
    return OK;
}

int BER::decodeUnbindRequestData(vector<char> &message){
    int whereLengthEnd;
    int length = decodeBerLength(message, whereLengthEnd);
    if(length > 0){
        return ERR;
    }
    return OK;
}

int BER::decodeSearchRequestBaseObject(vector<char> &message, ldap_msg_t &resultMessage){
    string baseObjects = decodeStr(message);
    istringstream stringStream(move(baseObjects));
    string baseObj;
    while(getline(stringStream, baseObj, DOMAIN_COMPONENT_SEPARATOR)){
        if(baseObj.rfind(DOMAIN_COMPONENT_PREFIX, STRING_BEGINNING)){
            return ERR;
        }
        resultMessage.SearchRequest.baseObject.push_back(move(baseObj));
    }
    return OK;    
}

int BER::decodeSearchRequestFilters(vector<char> &message, ldap_msg_t &resultMessage){
    try{
        resultMessage.SearchRequest.filter = decodeSearchRequestFiltersSwitch(message);
    }
    catch(int err){
        return err;
    }
    return OK;
}

filter_t BER::decodeSearchRequestFiltersSwitch(vector<char> &message){
    filter_t filter;
    unsigned char type = (unsigned char)message[position];
    try{
        switch(type){
            case FILTER_AND:
            case FILTER_OR:
            case FILTER_NOT:
                filter = decodeSearchFilterAndOrNot(message, (filter_type)type);
                break;
            case FILTER_SUBSTRING:
                filter = decodeSearchFilterSubstring(message);
                break;
            case FILTER_EQUALITY_MATCH:
                filter = decodeSearchFilterStringMatch(message);
                break;
            case NO_FILTER:
                filter.type = No_fltr;
                break;
            default:
                throw ERR;
        }
        return filter;
    }
    catch(int err){
        throw;
    }
}

filter_t BER::decodeSearchFilterSubstring(vector<char> &message){
    int whereLengthEnd;
    filter_t filter;
    filter.type = Fltr_substr;
    skipBerTagLength(message);
    string column;
    try{
        column = decodeStr(message);
    }
    catch(int err){
        throw;
    }
    if(message[position] != SEQUENCE){
        throw ERR;
    }
    int seqLen = decodeBerLength(message, whereLengthEnd);
    skipBerTagLength(message);
    int seqEnd = position + seqLen;
    while(position < seqEnd){
        filter_string_data_t data;
        int substringLength = decodeBerLength(message, whereLengthEnd);
        int substringStartPosition = position+whereLengthEnd;
        string value(message.begin()+substringStartPosition, message.begin()+substringStartPosition+substringLength);
        switch((unsigned char)message[position]){
            case SUBSTRING_STARTS_WITH:
                data.type = Substr_start;
                break;
            case SUBSTRING_CONTAINS:
                data.type = Substr_contains;
                break;
            case SUBSTRING_ENDS_WITH:
                data.type = Substr_end;
                break;
            default:
                throw ERR;
        }
        data.column = column;
        data.value = move(value);
        filter.data.push_back(move(data));
        position += whereLengthEnd + substringLength;
    }
    return filter;
}

filter_t BER::decodeSearchFilterStringMatch(vector<char> &message){
    filter_t filter;
    filter_string_data_t data;
    filter.type = Fltr_str_eq;
    data.type = Str_eq;
    skipBerTagLength(message);
    try{
        string column = decodeStr(message);
        string value = decodeStr(message);
        data.column = move(column);
        data.value = move(value);
    }
    catch(int err){
        throw;
    }
    filter.data.push_back(move(data));
    return filter;
}

filter_t BER::decodeSearchFilterAndOrNot(vector<char> &message, filter_type type){
    filter_t filter;
    filter.type = type;
    int whereLengthEnd;
    int length = decodeBerLength(message, whereLengthEnd);
    skipBerTagLength(message);
    int End = position + length;
    if((type == Fltr_or) || (type == Fltr_and)){
        while(position < End){
            filter.childs.push_back(decodeSearchRequestFiltersSwitch(message));
        }
    }
    else{
        filter.childs.push_back(decodeSearchRequestFiltersSwitch(message));
        if(position < End){
            throw ERR;
        }
    }
    return filter;
}

int BER::encode(vector<char> &resultMessage, ldap_msg_t &message){
    position = 0;
    resultMessage.clear();
    resultMessage.push_back(SEQUENCE);
    if(addMessageLength(resultMessage, message)){
        return ERR;
    }
    try{
        encodeInt(resultMessage, message.MsgId);
        switch(message.OpCode){
            case LDAP_BIND_RESPONSE:
                return encodeBindResponse(resultMessage, message);        
            case LDAP_SEARCH_RESULT_ENTRY:
                return encodeSearchResEntry(resultMessage, message);
            case LDAP_SEARCH_RESULT_DONE:
                return encodeSearchResDone(resultMessage, message);
            default:
                return ERR;
        }
    }
    catch(int err){
        return err;
    }
}

int BER::encodeBindResponse(vector<char> &resultMessage, ldap_msg_t &message){
    resultMessage.push_back(LDAP_BIND_RESPONSE);
    int responseLength = calculateBindResponseResDoneLength(message.BindResponse);
    addLength(resultMessage, responseLength);
    encodeEnumerated(resultMessage, message.BindResponse.resultCode);
    encodeString(resultMessage, message.BindResponse.matchedDN);
    encodeString(resultMessage, message.BindResponse.errorMessage);
    return OK;
}

int BER::encodeSearchResEntry(vector<char> &resultMessage, ldap_msg_t &message){
    resultMessage.push_back(LDAP_SEARCH_RESULT_ENTRY);
    int searchResEntryLength = calculateSearchResEntryLength(message.SearchResEntry);
    addLength(resultMessage, searchResEntryLength);
    encodeString(resultMessage, message.SearchResEntry.objName);
    encodeSearchResEntryAttributes(resultMessage, message.SearchResEntry.attributes);
    return OK;
}

int BER::encodeSearchResDone(vector<char> &resultMessage, ldap_msg_t &message){
    resultMessage.push_back(LDAP_SEARCH_RESULT_DONE);
    int responseLength = calculateBindResponseResDoneLength(message.SearchResDone);
    addLength(resultMessage, responseLength);
    encodeEnumerated(resultMessage, message.SearchResDone.resultCode);
    encodeString(resultMessage, message.SearchResDone.matchedDN);
    encodeString(resultMessage, message.SearchResDone.errorMessage);
    return OK;
}

int BER::addMessageLength(vector<char> &resultMessage, ldap_msg_t &message){
    int length = calculateIntLength(message.MsgId) + SHORT_FORM_HEADER_SIZE;
    int tmpLength;
    switch(message.OpCode){
        case LDAP_BIND_RESPONSE:
            tmpLength = calculateBindResponseResDoneLength(message.BindResponse);
            break;
        case LDAP_SEARCH_RESULT_ENTRY:
            tmpLength = calculateSearchResEntryLength(message.SearchResEntry);
            break;
        case LDAP_SEARCH_RESULT_DONE:
            tmpLength = calculateBindResponseResDoneLength(message.SearchResDone);
            break;
        default:
            return ERR;
    }
    length += calculateBerHeaderLength(tmpLength) + tmpLength;
    addLength(resultMessage, length);
    return OK;
}

void BER::encodeSearchResEntryAttributes(vector<char> &resultMessage, vector<search_result_entry_attribute_data_t> attributes){
    resultMessage.push_back(SEQUENCE);
    int length = calculateResEntryAttribsLength(attributes);
    addLength(resultMessage, length);
    for(size_t i = 0; i < attributes.size(); i++){
        resultMessage.push_back(SEQUENCE);
        int aLength = calculateResEntryAttribLength(attributes[i]);
        addLength(resultMessage, aLength);
        encodeString(resultMessage, attributes[i].type);
        resultMessage.push_back(SET);
        int valLength = calculateBerHeaderLength((int) attributes[i].value.size()) + attributes[i].value.size();
        addLength(resultMessage, valLength);
        encodeString(resultMessage, attributes[i].value);
    }
}

void BER::encodeInt(vector<char> &resultMessage, int value){
    resultMessage.push_back(INTEGER);
    int length = calculateIntLength(value);
    if(length > MAX_SHORT_FORM_LENGTH){
        addLongFormLength(resultMessage, length);
        addLongFormInt(resultMessage, value, length);
    }
    else if((length <= MAX_SHORT_FORM_LENGTH) && (length > 1)){
        resultMessage.push_back(length);
        addLongFormInt(resultMessage, value, length);
    }
    else{
        resultMessage.push_back(length);
        resultMessage.push_back(value);
    }
}

void BER::encodeEnumerated(vector<char> &resultMessage, int value){
    resultMessage.push_back(ENUMERATED);
    int length = calculateIntLength(value);
    if(length > MAX_SHORT_FORM_LENGTH){
        addLongFormLength(resultMessage, length);
        addLongFormInt(resultMessage, value, length);
    }
    else if((length <= MAX_SHORT_FORM_LENGTH) && (length > 1)){
        resultMessage.push_back(length);
        addLongFormInt(resultMessage, value, length);
    }
    else{
        resultMessage.push_back(length);
        resultMessage.push_back(value);
    }
}

void BER::encodeString(vector<char> &resultMessage, string value){
    resultMessage.push_back(STRING);
    int length = (int) value.size();
    addLength(resultMessage, length);
    for(size_t i = 0; i < value.size(); i++){
        resultMessage.push_back(value[i]);
    }
}

void BER::addLength(vector<char> &resultMessage, int length){
    if(length > MAX_SHORT_FORM_LENGTH){
        addLongFormLength(resultMessage, length);
    }
    else{
        resultMessage.push_back(length);
    }
}

void BER::addLongFormLength(vector<char> &resultMessage, int length){
    int lengthOfLength = calculateIntLength(length);
    resultMessage.push_back(lengthOfLength + LONG_FORM_FLAG);
    addLongFormInt(resultMessage, length, lengthOfLength);
}

void BER::addLongFormInt(vector<char> &resultMessage, int value, int length){
    int v;
    for(int i = length-1; i >= 0; i--){
        v = value & (BYTE_MASK << (i * BYTE));
        resultMessage.push_back((char) v);
    }
}

int BER::calculateBindResponseResDoneLength(bind_response_search_done_data_t &data){
    int resultCodeLength = calculateIntLength(data.resultCode); 
    int length = calculateBerHeaderLength(resultCodeLength) + resultCodeLength;
    length += calculateBerHeaderLength((int)data.matchedDN.size()) + data.matchedDN.size();
    length += calculateBerHeaderLength((int)data.errorMessage.size()) + data.errorMessage.size();
    return length;
}

int BER::calculateSearchResEntryLength(search_result_entry_data_t &searchResEntry){
    int length;
    length = calculateBerHeaderLength((int)searchResEntry.objName.size()) + searchResEntry.objName.size();
    int attributesLength = calculateResEntryAttribsLength(searchResEntry.attributes);
    length += calculateBerHeaderLength(attributesLength) + attributesLength;
    return length;
}

int BER::calculateResEntryAttribsLength(vector<search_result_entry_attribute_data_t> attributes){
    int attributesLength = 0;
    for(size_t i = 0; i < attributes.size(); i++){
        int aLength = calculateResEntryAttribLength(attributes[i]);
        attributesLength += calculateBerHeaderLength(aLength) + aLength;
    }
    return attributesLength;
}

int BER::calculateResEntryAttribLength(search_result_entry_attribute_data_t attribute){
    int typeLength = calculateBerHeaderLength((int) attribute.type.size()) + attribute.type.size();
    int valueLength = calculateBerHeaderLength((int) attribute.value.size()) + attribute.value.size();
    int setLength = calculateBerHeaderLength(valueLength) + valueLength;
    return typeLength + setLength;
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