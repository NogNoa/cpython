/* Module support interface */

extern object *initmodule PROTO((char *, struct methodlist *));
int getintarg PROTO((object *v,	int *a));
int getnoarg PROTO((object *v));
int getstrarg PROTO((object *v, object **a));
int getstrstrarg PROTO((object *v, object **a, object **b));
int getstrintarg PROTO((object *v, object **a, int *b));
int getlonglongargs PROTO((object *v, long *a, long  *b));
