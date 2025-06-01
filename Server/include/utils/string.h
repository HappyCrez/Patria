/**
 * @file  string.h
 * @brief header file has string definition and it's handling.
 */

#pragma once
#include "dependencies.h"

/**
 * @brief struct is represent char sequence in memory with it's length
 * @param data is pointer on char sequence
 * @param len is size of char sequence (without '\0')
 */
struct string
{
        char *data;
        int len;
};

/**
 * @brief initialize string (allocate memory by length+1)
 * @param length is size of string (without '\0')
 * @return pointer on new string
 */
struct string *string_init(int length);

/**
 * @brief initialize string by exisiting data and it's size
 * @param data is pointer on existing data
 * @param length is size of data (without '\0')
 * @return pointer on new string
 * @note If data size is not know set length param equal -1, then length of data will
 * be calculated by strlen function
 */
struct string *string_init_by_data(char *data, int length);

/**
 * @brief full copy string struct.
 * @return pointer on new string
 */
struct string *string_copy(struct string *string);

/**
 * @brief destroy string, free data and pointer on structure
 * @param string string that will be deleted
 * @note if string is created by existing data, it's may two free operations,
 * controll it (prefer is free memory by string_destroy, not separated free calls)
 */
void string_destroy(struct string *string);

/**
 * @brief compare structs string by it's symbols
 * @param str1 string1 to compare
 * @param str2 string2 to compare
 * @return zero if str1 == str2; 
 * -value when ith character in str1 is lower then in str2 (by ASCII code); 
 * +value when ith character in str1 is greater then in str2 (by ASCII code)
 * @note if strings are not equal, order to pass arguments is important,
 * change the order will return reversed value
 */
int string_compare(struct string *str1, struct string *str2);