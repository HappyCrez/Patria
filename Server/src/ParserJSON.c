#include "ParserJSON.h"

// request input format ptr to first " -> "field"
struct pair parseJSONField(char **request)
{
    struct pair response = {0ll, 0ll};

    (*request)++; // Skip -> "
    response.first = (ll)(*request);
    while (**request != '\0' && **request != '\"' && **request != ',')
    {
        (*request)++;      // Skip -> field_name
        response.second++; // Get field length
    }
    **request = '\0';
    *request += 2; // Skip -> ": OR Skip -> ",

    return response;
}

// request input format ptr to first " -> "field_name1":"field_val1"
// request output format ptr to first " -> "field_name2":"field_val2"
struct parsedData parseJSONRow(char **request)
{
    struct parsedData response = {{0ll, 0ll}, {0ll, 0ll}};
    
    response.field_name = parseJSONField(request);
    response.field_val = parseJSONField(request);

    return response;
}