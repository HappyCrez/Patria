#pragma once
#include "Dependencies.h"

typedef struct
{
    Pair *field_name; // JSON Data "field_name":"field_val"
    Pair *field_val;
} JSONData;

void InitJSONData(JSONData *ptr, Pair *field_name, Pair *field_val);
void DestroyJSONData(JSONData *ptr);

// request input format ptr to first " -> "field"
Pair *parseJSONField(char **request);

// Use parseJSONField function
// request input format ptr to first " -> "field_name1":"field_val1"
// request output format ptr to first " -> "field_name2":"field_val2"
JSONData *parseJSONRow(char **request);