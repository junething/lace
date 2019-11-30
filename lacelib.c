#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
typedef char* string;
#define new(type) (type *) calloc(1, sizeof(type))
string str_add(string one, string two) {
	string new = (string)calloc(1, sizeof(char) * strlen(one) + strlen(two) + 1);
	strcat(new, one);
	strcat(new, two);
	return new;
}
