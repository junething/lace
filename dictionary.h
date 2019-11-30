#ifndef DICT_HEADER
#define DICT_HEADER
#include "includes.h"
typedef struct
{
	/*const*/ char* key;
	void *value;
} DictItem;
typedef struct {
	int len;
	int cap;
	DictItem* array;
    void(*release)(void*);
} Dict;
//typedef Dict_s *Dictionary;

void* dict_find(Dict* dict, const char* key, bool *found);
void* dict_find_n(Dict* dict, const char* key, bool *found);
void dict_add(Dict* dict, const char* key, void* value);
void dict_remove_top_segment(Dict* dict);
void dict_new_division(Dict* dict);
Dict* dict_new(void);
#endif
