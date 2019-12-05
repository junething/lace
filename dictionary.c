#include "dictionary.h"
#include "includes.h"
#include "macros.h"
#include "types.h"
#include "main.h"
#include "utils.h"
const string DIVISION = "-<< SECTION BREAK >>-";
int dict_find_index(Dict* dict, const char* key) {
    for (int i = 0; i < dict->len; i++) {
        if (!strcmp(dict->array[i].key, key)) {
            return i;
        }
    }
    return -1;
}

void* dict_find(Dict* dict, const char* key, bool *found) {
    if(key == NULL) ERROR("Key NULL");
    int idx = dict_find_index(dict, key);
    if (idx != -1){
        if(found != NULL)
            *found = true;
        return dict->array[idx].value;
    }
    if(found != NULL)
        *found = false;
    else {
        hold();
        ERROR("'%s' not found", key);
    }
    return NULL;
}

void* dict_find_n(Dict* dict, const char* key, bool *found) {
    if(key == NULL) ERROR("Key NULL");
    for (int i = dict->len - 1; i >= 0; i--) {
        if (!strcmp(dict->array[i].key, key)) {
            if(found != NULL)
                *found = true;
            return dict->array[i].value;
        }
    }
    if(found != NULL)
        *found = false;
    else { 
        hold();
        ERROR("'%s' not found", key);
    }
    return NULL;
}
void dict_new_division(Dict* dict) { 
	dict_add(dict, DIVISION, NULL);
}
void dict_remove_top_segment(Dict* dict) {
    LOG("removed %s", DIVISION);
    for (int i = dict->len - 1; i >= 0; i--) {
        free(dict->array[i].key);
        //free(&dict->array[i]);
        //dict->array[i] = NULL;
        dict->len--;
        if (!strcmp(dict->array[i].key, DIVISION)) {
            return;
        }
    }
}
void dict_add(Dict* dict, const char* key, void* value) {
    int idx = dict_find_index(dict, key);
    if (idx != -1 && strcmp(key, DIVISION) != 0) {
        dict->array[idx].value = value;
        return;
    }
    if (dict->len == dict->cap) {
        dict->cap *= 2;
        dict->array = realloc(dict->array, dict->cap * sizeof(DictItem));
    }
    dict->array[dict->len].key = strdup(key);
    dict->array[dict->len].value = value;
    dict->len++;
    LOG("added '%s'", key);
}
Dict* dict_new(void) {
    Dict proto = {0, 10, malloc(10 * sizeof(DictItem))};
    Dict* d = malloc(sizeof(Dict));
    *d = proto;
    return d;
}

void dict_free(Dict* dict) {
    for (int i = 0; i < dict->len; i++) {
        free(dict->array[i].key);
    }
    free(dict->array);
    free(dict);
}
