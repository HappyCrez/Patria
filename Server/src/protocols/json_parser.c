#include "protocols/json.h"

struct json_parser *json_parser_init(struct string *json)
{
        struct json_parser *parser = malloc(sizeof(struct json_parser));
        parser->json = json;
        parser->field_name = null;
        parser->field_val = null;
}

void json_parser_destroy(struct json_parser *parser)
{
        if (parser->field_name != null)
                string_destroy(parser->field_name);
        if (parser->field_val != null)
                string_destroy(parser->field_val);
        free(parser);
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
        return null; // Quote not found
}

/**
 * @brief parse one field
 * @param parser pointer on parser
 * @return new string with field data
 */
struct string *json_parse_field(struct json_parser *parser)
{
        char *field_start = strchr(parser->json->data, '"'); // Field start from quote -> "
        if (field_start == null)
                return null; // Incorrect json
        ++field_start;       // Skip quote
        parser->json->data = field_start;

        char *field_end = strchr(parser->json->data, '"');
        if (field_end == null)
                return null; // Incorrect json
                
        /* Calculate field_name size */
        struct string *field = string_init(abs(field_end - field_start));
        strncpy(field->data, field_start, field->len);

        ++field_end;                    // Skip quote
        parser->json->data = field_end; //

        return field;
}

bool json_parser_execute(struct json_parser *parser)
{
        /* Before new allocation free memory */
        if (parser->field_name != null)
        {
                string_destroy(parser->field_name);
                parser->field_name = null;
        }

        if (parser->field_val != null)
        {
                string_destroy(parser->field_val);
                parser->field_val = null;
        }

        parser->field_name = json_parse_field(parser);
        parser->field_val = json_parse_field(parser);

        bool response = true;
        if (parser->field_name == null || parser->field_val == null)
                response = false;
        return response;
}