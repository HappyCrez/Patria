#include "parsers/json_parser.h"

void init_json_data(json_data *ptr, struct pair *field_name, struct pair *field_val)
{
    ptr->field_name = field_name;
    ptr->field_val = field_val;
}

void destroy_json_data(json_data *ptr)
{
    free(ptr->field_name);
    free(ptr->field_val);
    free(ptr);
}

struct pair *parse_json_field(char **request)
{
    struct pair *response = malloc(sizeof(struct pair));

    (*request)++; // Skip -> "
    response->first = (ll)(*request);
    while (**request != '\0' && **request != '\"' && **request != ',')
    {
        (*request)++;      // Skip -> field_name
        response->second++; // Get field length
    }
    **request = '\0';
    *request += 2; // Skip -> ": OR Skip -> ",

    return response;
}

json_data *parse_json_row(char **request)
{
    json_data *response   = malloc(sizeof(json_data));
    response->field_name = parse_json_field(request);
    response->field_val  = parse_json_field(request);
    return response;
}