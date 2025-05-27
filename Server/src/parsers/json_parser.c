#include "parsers/json_parser.h"

void json_parser_init(struct json_parser *ptr, char *json)
{
        ptr->json = json;
        ptr->field_name = NULL;
        ptr->field_val = NULL;
}

void json_parser_destroy(struct json_parser *ptr)
{
        free(ptr->field_name);
        free(ptr->field_val);
        free(ptr);
}

static char *find_closing_quote(char *start)
{
        char *ptr = start;
        while (*ptr)
        {
                if (*ptr == '\"')
                {
                        // Check quote on shield
                        int backslash_count = 0;
                        const char *q = ptr - 1;
                        while (q >= start && *q == '\\')
                        {
                                backslash_count++;
                                q--;
                        }

                        if (backslash_count % 2 == 0)
                        {
                                return ptr; // Not shielded
                        }
                        // Else continue search
                }
                ptr++;
        }
        return NULL; // Quote not found
}

int json_parser_execute(struct json_parser *parser)
{
        // Before new allocation free memory
        if (parser->field_name != NULL)
        {
                if ((char *)parser->field_name->first != NULL)
                        free((char *)parser->field_name->first);
                free(parser->field_name);
        }

        if (parser->field_val != NULL)
        {
                if ((char *)parser->field_val->first != NULL)
                        free((char *)parser->field_val->first);
                free(parser->field_val);
        }

        struct pair *field_name = malloc(sizeof(struct pair));
        struct pair *field_val = malloc(sizeof(struct pair));

        char *field_name_start = strchr(parser->json, '"'); // Field start from quote -> "
        if (field_name_start == NULL)
                return JSON_FILE_INCORRECT; // Incorrect json
        ++field_name_start;                 // Skip quote
        parser->json = field_name_start;

        char *field_name_end = strchr(parser->json, '"');
        if (field_name_end == NULL)
                return JSON_FILE_INCORRECT; // Incorrect json
        ++field_name_end;                   // Skip quote
        parser->json = field_name_end;      //

        field_name->second = abs((ll)field_name_end - (ll)field_name_start);      // Calculate field_name size
        field_name->first = (ll)malloc(field_name->second * sizeof(char));        //
        strncpy((char *)field_name->first, field_name_start, field_name->second); //
        ((char *)field_name->first)[field_name->second - 1] = '\0';               // Null-terminate

        char *field_val_start = strchr(parser->json, '\"');
        if (field_val_start == NULL)
                return JSON_FILE_INCORRECT; // Incorrect json
        ++field_val_start;                  // Skip quote
        parser->json = field_val_start;

        char *field_val_end = find_closing_quote(parser->json);
        if (field_val_start == NULL)
                return JSON_FILE_INCORRECT; // Incorrect json
        ++field_val_end;                    // Skip quote for next function call
        parser->json = field_val_end;

        field_val->second = abs((ll)field_val_end - (ll)field_val_start);      // Calculate field_val size
        field_val->first = (ll)malloc(field_val->second * sizeof(char));       //
        strncpy((char *)field_val->first, field_val_start, field_val->second); //
        ((char *)field_val->first)[field_val->second - 1] = '\0';              // Null-terminate

        parser->field_name = field_name;
        parser->field_val = field_val;
        return SUCCESS;
}