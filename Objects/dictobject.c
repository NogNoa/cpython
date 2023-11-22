#include "allobjects.h"
#include "PROTO.h"
#include "exmalloc.h"

typedef struct {
	OB_VARHEAD
	int phys_size; //actual allocated size for keys and objects
	char **dict_key; //listobject
	object **ob_item;
} dictobject;


object *
newdictobject(void)
{
	dictobject *op;
	op = (dictobject *) malloc(sizeof(dictobject));
	if (op == NULL) {
		return err_nomem();
	}
	op->dict_key = NULL;
	op->ob_item = NULL;
	NEWREF(op);
	op->ob_type = &Dicttype;
	op->ob_size = 0;
	op->phys_size = 0;
	return (object *) op;
}


static void
dict_dealloc(op)
	dictobject *op;
{
	int i;
	for (i = op->phys_size; i-- > 0;) {
		XDEL(op->dict_key[i]);
		XDECREF(op->ob_item[i]);
	}
	XDEL(op->dict_key);
	XDEL(op->ob_item);
	free((ANY *)op);
}

static int
keylookup(dp, key)
	dictobject *dp; 
	char *key;
{
	int i;
	for (i = 0; i < dp->ob_size; i++) {
		if (!strcmp(key, dp->dict_key[i]))
			return i;
	}
	return -1;
}

object *
dictlookup(dp, key)
	object *dp; 
	char *key;
{
	int i;
	dictobject* dct;
	if (!is_dictobject(dp)) {
		err_badcall();
		return NULL;
	}
	else {
		dct = (dictobject *) dp;
	}
	i = keylookup(dct, key);
	if (i >= 0)
		{return dct->ob_item[i];}
	else {
		// err_setstr(KeyError, key);
		return NULL;
	}	
}


static int
ins(self, key, item)
	dictobject *self;
	char *key;
	object *item;
{
	int where;
	char **keys;
	object **entries;
	if (key == NULL || item == NULL) {
		err_badcall();
		return -1;
	}
	where = (int) self->ob_size;
	if (where < 0)
		{where = 0;}
	self->ob_size++;
	keys = self->dict_key;
	entries = self->ob_item;
	if (self->phys_size < self->ob_size)
	{	self->phys_size = self->ob_size;
		RESIZE(keys, char *, self->phys_size);
		RESIZE(entries, object *, self->phys_size);
	}
	if (entries == NULL) {
		err_nomem();
		return -1;
	}
	keys[where] = NEW(char, strlen(key)+1);
	strcpy(keys[where], key);
	entries[where] = item;
	self->dict_key = keys;
	self->ob_item = entries;
	INCREF(item);
	return 0;
}

static int
replace(self, where, item)
	dictobject *self;
	int where;
	object *item;
{
	object **entries = self->ob_item;
	if (where >= self->ob_size) {
		err_badcall();
		return -1;
	}
	DECREF(entries[where]);
	entries[where] = item;
	INCREF(item);
	self->ob_item = entries;
	return 0;
}

int
dictinsert(dp, key, item)
	object *dp, *item;
	char *key;
{
	int where;
	dictobject* dct;
	if (!is_dictobject(dp)) {
		err_badcall();
		return -1;
	}
	else {
		dct = (dictobject *) dp;
	}
	if (key == NULL || item == NULL) {
		err_badarg();
		return -1;
	}
	where = keylookup(dct, key);
	if (where >= 0) {
		err_badcall();
		return -1;
	}
	else {
		return ins(dct, key, item);
	}
}


static int
rem(self, where)
dictobject *self;
int where;
{
	object **entries = self->ob_item;
	char **keys = self->dict_key;
	if (where < 0 || self->ob_size < where) {
		err_badcall();
		return -1;
	}
	self->ob_size--;
	for (; where < self->ob_size; where++) {
		entries[where] = entries[where+1];
	}
	DEL(keys[where]);
	DECREF(entries[where]);
	entries[where] = 0;
	self->dict_key = keys;
	self->ob_item = entries;
	return 0;
}

int
dictremove(dp, key)
	object *dp;
	char *key;
{
	int i;
	dictobject* dct;
	if (!is_dictobject(dp)) {
		err_badcall();
		return -1;
	}
	else {
		dct = (dictobject *) dp;
	}
	i = keylookup(dct, key);
	if (i >= 0)
		{return rem(dct,i);}
	else {
		err_setstr(KeyError, key);
		return -1;
	}
}

/* nognoa: I no longer resize on shrinking the dictionary
		   code always calls import.cleardict which runs
		   dictremove on entries one by one. before just
		   deallocating the dictionary. This is wastefull
		   with all the reallocs. And in general adding and
		   removing entries can cause the dictionary to need to
		   be moved in memory for no reason.
		   Increase size as needed. Decrease when twice as large as needed.
*/


int
getdictsize(dp)
	object *dp;
{
	if (!is_dictobject(dp)) {
		err_badcall();
		return -1;
	}
	else
		return ((dictobject *)dp) -> ob_size;
}


char *
getdictkey(dp, i)
	object *dp;
	int i;
{
	dictobject *dct;
	if (!is_dictobject(dp)) {
		err_badcall();
		return NULL;
	}
	else
		{dct = (dictobject *) dp;}
	if (i < 0 || dct->ob_size < i) {
		err_badarg();
		return NULL;
	}
	return dct->dict_key[i];
}

static object * //listobject*
keylist(dp)
	dictobject *dp;
{
	int i;
	object *back = newlistobject(0);
	object *str;
	for (i=0;i < dp->ob_size; i++)
	{
		str = newstringobject(dp->dict_key[i]);
		addlistitem(back, str);
	}
	return back;
}

object * //listobject*
getdictkeys(dp)
	object *dp;
{
	if (!is_dictobject(dp)) {
		err_badcall();
		return NULL;
	}
	else {
		dictobject *dct = (dictobject *) dp;
		return keylist(dct);
	}

}

static void
dict_print(op, fp, flags)
	dictobject *op;
	FILE *fp;
	int flags;
{
	int i;
	fprintf(fp, "{");
	for (i = 0; i < op->ob_size && !StopPrint; i++) {
		fprintf(fp, "%s: ", op->dict_key[i]);
		printobject(op->ob_item[i], fp, flags);
		fprintf(fp, (i < op->ob_size-1) ? ", " : "}");
	}
}


static object *
dict_repr(op)
	dictobject *op;
{
	object *s, *k, *v, *t, *comma, *colon;
	int i;
	s = newstringobject("{");
	comma = newstringobject(", ");
	colon = newstringobject(": ");
	t = newstringobject("}");
	for (i = 0; i < op->ob_size && s != NULL; i++) {
		k = newstringobject(op->dict_key[i]);
		v = reprobject(op->ob_item[i]);
		joinstring(&k, colon);
		joinstring(&v, (i < op->ob_size-1) ? comma : t);
		joinstring(&k, v);
		joinstring(&s, k);
		DECREF(k);
		DECREF(v);
	}
	DECREF(comma);
	DECREF(colon);
	DECREF(t);
	return s;
}



static int
dict_length(a)
	dictobject *a;
{
	return a->ob_size;
}

static object * //item
dict_subscript(dp, str)
	object *dp; //dictobject
	object *str; //stringobject
{
	int i;
	dictobject* dct;
	char *key;
	if (!is_dictobject(dp)) {
		err_badcall();
		return NULL;
	}
	else {
		dct = (dictobject *) dp;
	}
	if (!is_stringobject(str)) {
		err_badcall();
		return NULL;
	}
	else {
		key = ((stringobject *) str)->ob_sval;
	}
	i = keylookup(dct, key);
	if (i >= 0) {
		INCREF(dct->ob_item[i]);
		return dct->ob_item[i];
	}
	else {
		// err_setstr(KeyError, key);
		return NULL;
	}
}

static int //error
dict_ass_subscript(dp, str, item)
	object *dp; //dictobject
	object *str; //string_object
	object *item; //item
{
	dictobject* dct;
	int where;
	char *key;
	if (is_listobject(dp))
		dct = (dictobject *) dp;
	else {
		err_badarg();
		return -1;
	}
	if (!is_stringobject(str)) {
		err_badcall();
		return -1;
	}
	else {
		key = ((stringobject *) str)->ob_sval;
	}
	where = keylookup(dct, key);
	if (where >= 0) {
		return replace(dct, where, item);
	}
	else {
		return ins(dct, key, item);
	}
}

static mapping_methods dict_as_mapping = {
	dict_length,		/*mp_length*/
	dict_subscript,		/*mp_subscript*/
	dict_ass_subscript,	/*mp_ass_subscript*/
};

typeobject Dicttype = {
	OB_HEAD_INIT(&Typetype)
	0, /* Number of items for varobject */
	"Dictionary",
	sizeof(dictobject),
	0, /* Item size for varobject */
	dict_dealloc,	/*tp_dealloc*/
	dict_print,	/*tp_print*/
	NULL,//dict_getattr,	/*tp_getattr*/
	NULL,		/*tp_setattr*/
	NULL,	/*tp_compare*/
	dict_repr,	/*tp_repr*/
	NULL,		/*tp_as_number*/
	NULL,//&dict_as_sequence,	/*tp_as_sequence*/
	&dict_as_mapping,		/*tp_as_mapping*/
};
