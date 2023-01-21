#include <malloc.h>

#define NEW(type, size) ((*type) malloc(size))
#define DEL(obj) (free(obj))
