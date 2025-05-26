#include "FileHelper.h"
#include "Algoritms.h"

Pair *createHTTPResponse(char* filename)
{
    Pair *response = malloc(sizeof(Pair));
    memset(response, 0, sizeof(response));

    // Open the file in read mode
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        return response;
    }

    // Go to the end of the file
    if (fseek(fp, 0L, SEEK_END) != 0)
    {
        fclose(fp);
        return response;
    }

    // Get the size of the file
    long file_size = ftell(fp);
    if (file_size == -1)
    {
        fclose(fp);
        return response;
    }
    
    // Go back to the beginning of the file
    if (fseek(fp, 0L, SEEK_SET) != 0)
    {
        fclose(fp);
        return response;
    }

    // Allocate memory for the buffer
    char *buffer = malloc(HTTP_RESPONSE_HEADER_SIZE+file_size+1);
    strcpy(buffer,HTTP_RESPONSE_HEADER);

    // Read the entire file into the buffer
    size_t read_size = fread(buffer+HTTP_RESPONSE_HEADER_SIZE-1, sizeof(char), file_size, fp);
    buffer[HTTP_RESPONSE_HEADER_SIZE+file_size-1] = '\0';

    // Verify succefull file read operation
    if (ferror(fp) != 0)
    {
        fclose(fp);
        free(buffer);
        return response;
    }
    fclose(fp);

    response->first = (ll)buffer;
    response->second = HTTP_RESPONSE_HEADER_SIZE+file_size-1;

    return response;
}