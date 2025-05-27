#pragma once
#include "dependencies.h"

#define JSON_ROW_FORMAT_LEN 13
#define JSON_ROW_FORMAT "{\"%s\": \"%s\"}"

#define JSON_TWO_ROW_FORMAT_LEN 25
#define JSON_TWO_ROW_FORMAT "{\"%s\": \"%s\", \"%s\": \"%s\"}"

struct json_parser
{
        char *json;
        struct pair *field_name; // JSON Data "field_name":"field_val"
        struct pair *field_val;
};

void json_parser_init(struct json_parser *ptr, char *json);
void json_parser_destroy(struct json_parser *ptr);

/**
 * @parser: ptr on struct json_parser
 * Every execute free old data if exists
 * Return code value SUCCESS || ERROR
 */
int json_parser_execute(struct json_parser *parser);