#include "Algoritms.h"

bool stringCompare(char *str1, char *str2)
{
    bool is_the_same = TRUE;
    for (int i = 0; *(str1+i) != '\0' && *(str2+i) != '\0'; ++i)
    {
        if (*(str1+i) != *(str2+i))
        {
            is_the_same = FALSE;
            break;
        }
    }
    return is_the_same;
}