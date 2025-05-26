#include "Parsers/JSONParser.h"

void InitJSONData(JSONData *ptr, Pair *field_name, Pair *field_val)
{
    ptr->field_name = field_name;
    ptr->field_val = field_val;
}

void DestroyJSONData(JSONData *ptr)
{
    free(ptr->field_name);
    free(ptr->field_val);
    free(ptr);
}

Pair *parseJSONField(char **request)
{
    Pair *response = malloc(sizeof(Pair));

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

JSONData *parseJSONRow(char **request)
{
    JSONData *response   = malloc(sizeof(JSONData));
    response->field_name = parseJSONField(request);
    response->field_val  = parseJSONField(request);
    return response;
}