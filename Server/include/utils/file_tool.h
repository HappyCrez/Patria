/**
 * @file  file_tool.h
 * @brief File tool provide interface to easy read files. 
 */

#define pragma once
#include "dependencies.h"
#include "string.h"

/**
 * @brief read file and return it content in string
 * @param filename is pointer on filepath value
 * @return file content 
 */
struct string *file_read(struct string *filename);