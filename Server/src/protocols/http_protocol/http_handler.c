#include "protocols/http.h"

/**
 * @brief define what type of http request
 * @param request http request data
 * @return get type (websocket upgrade, get resource)
 */
static enum http_get_types http_get_type(struct string *request)
{
        enum http_get_types get_type = STD_URL;
        if (strstr(request->data, HTTP_WS))
        {
                get_type = WS_URL;
        }
        return get_type;
}

/**
 * @brief function to return resource from server as http response
 * @param request http request data
 * @return http response with resource data
 */
static struct string *http_get_response(struct string *request)
{
        /* pointer on requested resource */
        char *filepath = strstr(request->data, HTTP_GET) + sizeof(HTTP_GET);
        int url_size = 0;
        while (filepath[url_size] != ' ') /* url end by whitespace */
        {
                url_size++;
        }

        /* url to any catalogs is parsed as access to messenger */
        struct string *resource;
        if (filepath[url_size - 1] == '/')
        {
                resource = string_init(sizeof(HTML_FILEPATH));
                strncpy(resource->data, HTML_FILEPATH, resource->len);
        }
        else
        {
                resource = string_init(sizeof(CLIENT_PATH) - 1 + url_size);
                strcpy(resource->data, CLIENT_PATH);
                strncpy(resource->data + sizeof(CLIENT_PATH) - 1, filepath, url_size);
        }
        struct string *response = http_build_response(resource);
        string_destroy(resource);
        return response;
}

/**
 * @brief function to create http upgrade response
 * @param request http request data
 * @return http response on upgrade
 */
static struct string *http_upgrade_response(struct string *request)
{
        char *guidkey = malloc(GUIDKEY_LEN);

        /* search key in request */
        char *key_field_val = strstr(request->data, WS_KEY_FIELD) + sizeof(WS_KEY_FIELD) - 1;
        if (key_field_val == null)
                return null;

        strncpy(guidkey, key_field_val, WS_KEY_LEN);
        guidkey[WS_KEY_LEN] = '\0';
        strcat(guidkey, GUID);

        unsigned char *sha1_code = malloc(SHA_DIGEST_LENGTH);
        SHA1(guidkey, strlen(guidkey), sha1_code);

        char *key = base64_encode(sha1_code, strlen(sha1_code));
        int key_len = strlen(key);

#define END "\n\n"
        struct string *response = string_init(sizeof(HTTP_WS_RESPONSE) - 1 + key_len + sizeof(END) - 1);

        strcpy(response->data, HTTP_WS_RESPONSE);
        strcpy(response->data + sizeof(HTTP_WS_RESPONSE) - 1, key);
        strcpy(response->data + sizeof(HTTP_WS_RESPONSE) - 1 + key_len, END);
#undef END

        free(guidkey);
        free(sha1_code);
        free(key);

        return response;
}

enum http_request http_request_type(struct string *request)
{
        enum http_request request_type = UNKNOWN_REQUEST;
        if (strstr(request->data, HTTP_GET))
                request_type = GET_REQUEST;
        else if (strstr(request->data, HTTP_POST))
                request_type = POST_REQUEST;
        return request_type;
}

struct http_response *http_handle_get(struct string *request)
{
        struct http_response *http = malloc(sizeof(struct http_response));

        switch (http_get_type(request))
        {
        case STD_URL:
                http->is_ws = false;
                http->response = http_get_response(request);
                break;
        case WS_URL:
                http->is_ws = true;
                http->response = http_upgrade_response(request);
                break;
        }
        return http;
}