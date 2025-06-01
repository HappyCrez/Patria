#include "utils/file_tool.h"

struct string *file_read(struct string *filename)
{
        FILE *fp = fopen(filename->data, "rb");
        if (fp == null)
        {
                return null;
        }

        /* Move to end of the file */
        if (fseek(fp, 0L, SEEK_END) != 0)
        {
                fclose(fp);
                return null;
        }

        /* Calculate file size */
        long file_size = ftell(fp);
        if (file_size == -1)
        {
                fclose(fp);
                return null;
        }

        /* Move to start of the file */
        if (fseek(fp, 0L, SEEK_SET) != 0)
        {
                fclose(fp);
                return null;
        }

        struct string *file_content = string_init(file_size);
        fread(file_content->data, sizeof(char), file_size, fp);

        /* Verify correct read operation */
        if (ferror(fp) != 0)
        {
                fclose(fp);
                string_destroy(file_content);
                return null;
        }
        fclose(fp);
        return file_content;
}