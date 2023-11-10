#include <stdio.h>
#include <string.h>
#include "errcode.h"
#include "exmalloc.h"

int fgets_intr(str, n, stream)
char *str; int n; FILE *stream;
{
    char * call = NEW(char, n+1);
    int back;
    strncpy(call, str, n);
    if (fgets(str, n, stream) == NULL)
    {   if (strncmp(call, str, n))
            {back = E_TOKEN;}
        else {back = E_EOF;}
    }
    else {back = E_OK;}
    DEL(call);
    return back;
}