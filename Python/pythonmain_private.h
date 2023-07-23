static void finaloutput PROTO((void));
static int askyesno PROTO((char *prompt));
int isatty PROTO((int fd));

extern grammar gram; /* From graminit.c */


#ifdef DEBUG
int debugging; /* Needed by parser.c */
#endif
