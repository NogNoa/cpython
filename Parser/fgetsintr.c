#include <stdio.h>
#include <string.h>
#include "errcode.h"

int fgetintr(str, n, stream)
char *str; int n; FILE *stream;
{
    char * call;
    strncopy(call, str, n);
    if (fgets(str, n, stream) == NULL)
    {   if (strcmp(call,str))
            {return E_TOKEN;}
        else {return E_EOF;}
    }
    else {return E_OK;}
}