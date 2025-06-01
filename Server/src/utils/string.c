#include "utils/string.h"


struct string *string_init(int length)
{
        struct string *string = malloc(sizeof(struct string));
        string->data = malloc(length + 1);
        string->data[length] = '\0';
        string->len = length;
        return string;
}

struct string *string_init_by_data(char *data, int length)
{
        struct string *string = malloc(sizeof(struct string));
        string->data = data;
        string->data[length] = '\0';
        string->len = length;
        return string;
}

struct string *string_copy(struct string *string)
{
        struct string *new_string = malloc(sizeof(struct string));
        new_string->data = malloc(string->len + 1);
        strncpy(new_string->data, string->data, string->len);
        new_string->data[string->len] = '\0';
        new_string->len = string->len;
        return new_string;
}

void string_destroy(struct string *string)
{
        free(string->data);
        free(string);
}

int string_compare(struct string *str1, struct string *str2)
{
        return strcmp(str1->data, str2->data);
}
