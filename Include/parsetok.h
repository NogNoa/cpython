/* Parser-tokenizer link interface */

#ifndef NODE
	#include "node.h"
	#define NODE
#endif

extern int parsestring PROTO((char *, grammar *, int start, node **n_ret));
extern int parsefile PROTO((FILE *, char *, grammar *, int start,
					char *ps1, char *ps2, node **n_ret));
