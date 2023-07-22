#include "allobjects.h"
#include "PROTO.h"
#include "exmalloc.h"

typedef struct {
	OB_VARHEAD
	object *dict_key; //listobject
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
	op->dict_key = newlistobject(0);
	op->ob_item = NULL;
	NEWREF(op);
	op->ob_type = &Dicttype;
	op->ob_size = 0;
	return (object *) op;
}


static void
dict_dealloc(op)
	dictobject *op;
{
	int i;
	DECREF(op->dict_key);
	for (i = 0; i < op->ob_size; i++) {
		if (op->ob_item[i] != NULL)
			free(op->ob_item[i]);
	}
	if (op->ob_item != NULL)
		free((ANY *)op->ob_item);
	free((ANY *)op);
}

static int
keylookup(dp, key)
	dictobject *dp; 
	char *key;
{
	int i;
	for (i = 0; i < dp->ob_size; i++) {
		if (!strcmp(key, (char *) getlistitem(dp->dict_key, i)))
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
		err_setstr(KeyError, key);
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
	object **entries;
	if (key == NULL || item == NULL) {
		err_badcall();
		return -1;
	}
	entries = self->ob_item;
	RESIZE(entries, object *, self->ob_size+1);
	if (entries == NULL) {
		err_nomem();
		return -1;
	}
	where = (int) self->ob_size;
	if (where < 0)
		where = 0;
	addlistitem(self->dict_key, (object *)key); //I guess
	entries[where] = item;
	self->ob_item = entries;
	INCREF(item);
	self->ob_size++;
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
		return replace(dct, where, item);
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
	object *entr;
	if (where < 0 || self->ob_size < where) {
		err_badcall();
		return -1;
	}
	remlistitem(self->dict_key, where);
	entr = entries[where];
	self->ob_size--;
	for (; where < self->ob_size; where++) {
		entries[where] = entries[where+1];
	}
	DECREF(entr);
	RESIZE(entries, object *, self->ob_size);
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
	dictobject *dct = (dictobject *) dp;
	if (i < 0 || dct->ob_size < i) {
		err_badarg();
		return NULL;
	}
	return (char *) getlistitem(dct->dict_key, i);
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
		fprintf(fp, "%s: ", (char *) getlistitem(op->dict_key, i));
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
		k = newstringobject((char *) getlistitem(op->dict_key, i));
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
	NULL,//&dict_as_mapping,		/*tp_as_mapping*/
};