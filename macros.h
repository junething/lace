#define new(type) (type *) calloc(1, sizeof(type))
#define ERROR(message) \
        error(1, __FILE__, __LINE__, message);
