#include "test.h"
#include "parsers/json_parser.h"

int test_json_parser()
{
        int code = TEST_SUCCESS;
        char json_row[] = "{\"name\": \"value\"}\0";
        char *row = json_row;
        char complex_json_row[] = "{\"name\": \"value\"val\\str  . , :\"}\0";
        char *row2 = complex_json_row;
        
        json_data *data1 = parse_json_row(&row);
        json_data *data2 = parse_json_row(&row2);
        
        if ((strcmp((char *)data1->field_name->first, "name") != 0 || data1->field_name->second != 5) &&
        (strcmp((char *)data1->field_val->first, "value") != 0 || data1->field_val->second != 6))
        {
                printf("\nTEST JSON #1 FAILED:\n");
                printf("field_name: \"%s\" ; expected: \"%s\"\n", (char *)data1->field_name->first, "name");
                printf("len: \"%lld\" ; expected: \"%d\"\n", data1->field_name->second, 4);
                
                printf("field_val: \"%s\" ; expected: \"%s\"\n", (char *)data1->field_val->first, "value");
                printf("len: \"%lld\" ; expected: \"%d\"\n", data1->field_name->second, 5);
                
                code = TEST_FAILED;
        }
        else
        {
                printf("TEST JSON #1 OK\n");
        }
        
        if ((strcmp((char *)data2->field_name->first, "name") != 0 || data2->field_name->second != 5) &&
        (strcmp((char *)data2->field_val->first, "value\"val\\str  . , :\"") != 0 || data2->field_val->second != 22))
        {
                printf("\nTEST JSON #2 FAILED:\n");
                printf("field_name: \"%s\" ; expected: \"%s\"\n", (char *)data2->field_name->first, "name");
                printf("len: \"%lld\" ; expected: \"%d\"\n", data2->field_name->second, 4);
                
                printf("field_val: \"%s\" ; expected: \"%s\"\n", (char *)data2->field_val->first, "value\"val\\str  . , :\"");
                printf("len: \"%lld\" ; expected: \"%d\"\n", data2->field_name->second, 21);
        
                code = TEST_FAILED;
        }
        else
        {
                printf("TEST JSON #2 OK\n");
        }
        return code;
}