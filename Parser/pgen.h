/* Parser generator interface */

#ifndef NODE
	#include "node.h"
	#define NODE
#endif

extern grammar gram;

extern grammar *meta_grammar PROTO((void));
extern grammar *pgen PROTO((struct _node *));
