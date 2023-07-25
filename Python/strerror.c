/* PD implementation of strerror() for systems that don't have it.
   Author: Guido van Rossum, CWI Amsterdam, Oct. 1990, <guido@cwi.nl>. */

#include <stdio.h>

extern int sys_nerr;
extern const char *const sys_errlist[];

const char *
strerror(err)
	int err;
{
	static char buf[20];
	if (err >= 0 && err < sys_nerr)
		return sys_errlist[err];
	sprintf(buf, "Unknown errno %d", err);
	return buf;
}

#ifdef THINK_C
int sys_nerr = 0;
char *sys_errlist[1] = 0;
#endif
