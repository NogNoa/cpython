#include <malloc.h>

#define NEW(type, size) ((type*) malloc(size))
#define DEL(obj) (free(obj))
#define RESIZE(item, type, ob_size) (item = (type*) realloc(item, ob_size))

#define XDEL(obj) if ((obj) == NULL) ; else DEL(obj)

#define ANY void

