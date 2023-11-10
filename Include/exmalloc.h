#include <malloc.h>

#define NEW(type, size) ((type*) malloc(sizeof(type) * size))
#define DEL(obj) {(free(obj)); ((obj) = NULL);}
#define RESIZE(item, type, ob_size) (item = (type*) realloc(item, sizeof(type) * ob_size))

#define XDEL(obj) {if ((obj) == NULL) ; else DEL(obj);}

#define ANY void

