#include "types.h"
#ifndef UTILS_HEADER
#define UTILS_HEADER
bool valint(const string str, int *out);
bool valfloat(const string str, float *out);
void log_lable(string file, int line);
void hold(void);
List* list_new(void);
#endif
