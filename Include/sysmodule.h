/* System module interface */

object *sysget PROTO((char *));
int sysset PROTO((char *, object *));
FILE *sysgetfile PROTO((char *, FILE *));
void initsys PROTO((void));
void setpythonpath PROTO((char *path));
void setpythonargv PROTO((int argc, char **argv));
void donesys PROTO((void));
	
