#ifndef FILTER_H
#define FILTER_H

#include <vector>
#include <string>

#define FILTER_AND 0xa0
#define FILTER_OR 0xa1
#define FILTER_NOT 0xa2
#define FILTER_EQUALITY_MATCH 0xa3
#define FILTER_SUBSTRING 0xa4

#define SUBSTRING_STARTS_WITH 0x80
#define SUBSTRING_CONTAINS 0x81
#define SUBSTRING_ENDS_WITH 0x82

using namespace std;

enum filter_type{
    Fltr_and = FILTER_AND,
    Fltr_or = FILTER_OR,
    Fltr_not = FILTER_NOT,
    Fltr_str_eq = FILTER_EQUALITY_MATCH,
    Fltr_substr = FILTER_SUBSTRING
};

enum filter_string_type{
    Str_eq = FILTER_EQUALITY_MATCH,
    Substr_start = SUBSTRING_STARTS_WITH,
    Substr_contains = SUBSTRING_CONTAINS,
    Substr_end = SUBSTRING_ENDS_WITH
};

typedef struct{
    filter_string_type type;
    string column;
    string value;
}filter_string_data_t;

typedef struct fltr{
    filter_type type;
    vector<struct fltr> childs;
    vector<filter_string_data_t> data;
}filter_t;

#endif