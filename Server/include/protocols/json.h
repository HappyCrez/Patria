/**
 * @file  json.h
 * @brief describes json builder and parser functions
 * @todo json_builder
 */

#pragma once
#include "dependencies.h"
#include "utils/string.h"

/* temporary solution to format one row jsons */
#define JSON_ROW_FORMAT "{\"%s\": \"%s\"}"

/* temporary solution to format two rows jsons */
#define JSON_TWO_ROW_FORMAT "{\"%s\": \"%s\", \"%s\": \"%s\"}"

#define JSON_PARSE_ERROR 1

/**
 * @brief json parser
 * @param json string to parse (parsing doesn't modify original string)
 * @param field_name data in first part of row "name":"val", ...
 * @param field_val data in second part of row
 */

struct json_parser
{
        struct string *json;
        struct string *field_name; /* public field (parse result) */
        struct string *field_val;  /* public field (parse result) */
};

/**
 * @brief create parser
 * @param json string to parse
 * @return pointer on new json_parser
 */
struct json_parser *json_parser_init(struct string *json);

/**
 * @brief current destroy json parser
 * @param parser pointer on json_parser to destruct
 */
void json_parser_destroy(struct json_parser *parser);

/**
 * @brief parse one row, parsed data is write into json_parser
 * @parser: ptr on struct json_parser
 * Every execute free old data if exists
 * @return true if row was parsed and false otherwise
 */
bool json_parser_execute(struct json_parser *parser);