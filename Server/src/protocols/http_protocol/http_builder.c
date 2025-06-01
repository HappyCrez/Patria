#include "protocols/http.h"

struct string *http_404()
{
        struct string *string = string_init(sizeof(HTTP_404) - 1);
        strncpy(string->data, HTTP_404, string->len);
        return string;
}

struct string *http_build_response(struct string *filename)
{
        /* prevent access to hidden files or directories */
        if (strstr(filename->data, "/.") != null)
        {
                return http_404();
        }

        struct string *file_content = file_read(filename);

        if (file_content == null)
        {
                return http_404();
        }

        struct string *response = string_init(sizeof(HTTP_200) - 1 + file_content->len);
        strncpy(response->data, HTTP_200, sizeof(HTTP_200));
        memcpy(response->data + sizeof(HTTP_200) - 1, file_content->data, file_content->len);
        string_destroy(file_content);
        return response;
}