
#include "includes.h"
#include "types.h"
#include "main.h"
#include "macros.h"
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
bool valfloat(const string str, float *out) {
    char *end;
    double value = strtod(str, &end);
    if (end == str || *end != '\0' || errno == ERANGE) {
        return false;
    }
    if(out != NULL) {
        *out = (float)value;
    }
    return true;
}
void log_lable(string file, int line) {
    fprintf(stderr, "\033[35;1m[%s:%d]\033[0m:\t", file, line);
}
void hold(void) {
    return;
}
List* list_new(void) {
    List* list = new (List);
    list->start = new (ListNode);
    list->end = list->start;
    return list;
}
