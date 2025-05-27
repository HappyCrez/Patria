#pragma once
#include "dependencies.h"

#define JSON_ROW_FORMAT_LEN 13
#define JSON_ROW_FORMAT "{\"%s\": \"%s\"}"

typedef struct
{
    struct pair *field_name; // JSON Data "field_name":"field_val"
    struct pair *field_val;
} json_data;

void json_data_init(json_data *ptr, struct pair *field_name, struct pair *field_val);
void json_data_destroy(json_data *ptr);

// request input format ptr to first " -> "field"
struct pair *parse_json_field(char **request);

/**
 * @request: input format ptr to first " -> "field_name1":"field_val1"
 * request will be change to ptr on next json row at the same way
 * Return: json_data
 */
json_data *parse_json_row(char **request);