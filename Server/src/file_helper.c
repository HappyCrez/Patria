#include "file_helper.h"

struct pair *response_http_404()
{
        struct pair *response = malloc(sizeof(struct pair));
        response->first = (ll)malloc(HTTP_404_HEADER_LEN);
        response->second = HTTP_404_HEADER_LEN;
        strcpy((char *)response->first, HTTP_404_HEADER);
        return response;
}

struct pair *createHTTPResponse(char *filename)
{
        if (strstr(filename, "/.") != NULL)
        {
                return response_http_404();
        }

        // Open the file in read mode
        FILE *fp = fopen(filename, "r");
        if (fp == NULL)
        {
                return response_http_404();
        }

        // Go to the end of the file
        if (fseek(fp, 0L, SEEK_END) != 0)
        {
                fclose(fp);
                return response_http_404();
        }

        // Get the size of the file
        long file_size = ftell(fp);
        if (file_size == -1)
        {
                fclose(fp);
                return response_http_404();
        }

        // Go back to the beginning of the file
        if (fseek(fp, 0L, SEEK_SET) != 0)
        {
                fclose(fp);
                return response_http_404();
        }

        // Allocate memory for the buffer
        char *buffer = malloc(HTTP_RESPONSE_HEADER_SIZE + file_size + 1);
        strcpy(buffer, HTTP_RESPONSE_HEADER);

        // Read the entire file into the buffer
        size_t read_size = fread(buffer + HTTP_RESPONSE_HEADER_SIZE - 1, sizeof(char), file_size, fp);
        buffer[HTTP_RESPONSE_HEADER_SIZE + file_size - 1] = '\0';

        // Verify succefull file read operation
        if (ferror(fp) != 0)
        {
                fclose(fp);
                free(buffer);
                return response_http_404();
        }
        fclose(fp);

        struct pair *response = malloc(sizeof(struct pair));
        response->first = (ll)buffer;
        response->second = HTTP_RESPONSE_HEADER_SIZE + file_size - 1;

        return response;
}