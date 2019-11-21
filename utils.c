
#include "includes.h"
#include "types.h"
bool valint(const string str, int *out)
{
    char *end;
    long value = strtol(str, &end, 10);
    if (end == str || *end != '\0' || errno == ERANGE) {
        return false;
    }
    if(out != NULL) {
        *out = (int)value;
    }
    return true;
}
