#include "algoritms/common_base_algoritms.h"

int string_compare(struct pair *val1, struct pair *val2)
{
        return strcmp((char *)val1->first, (char *)val2->first);
}