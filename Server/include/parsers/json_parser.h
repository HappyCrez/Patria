#pragma once
#include "dependencies.h"

typedef struct
{
    struct pair *field_name; // JSON Data "field_name":"field_val"
    struct pair *field_val;
} json_data;

void init_json_data(json_data *ptr, struct pair *field_name, struct pair *field_val);
void destroy_json_data(json_data *ptr);

// request input format ptr to first " -> "field"
struct pair *parse_json_field(char **request);

// Use parse_json_field function
// request input format ptr to first " -> "field_name1":"field_val1"
// request output format ptr to first " -> "field_name2":"field_val2"
json_data *parse_json_row(char **request);