#include "allobjects.h"
#include "PROTO.h"
#include "exmalloc.h"

typedef struct {
	OB_VARHEAD
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
	return (object *) op;
}


static void
dict_dealloc(op)
	dictobject *op;
{
	int i;
	for (i = 0; i < op->ob_size; i++) {
		if (op->dict_key[i] != NULL)
			free(op->dict_key[i]);
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
	RESIZE(keys, char *, self->ob_size);
	entries = self->ob_item;
	RESIZE(entries, object *, self->ob_size);
	if (entries == NULL) {
		err_nomem();
		return -1;
	}
	keys[where] = key;
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
	RESIZE(keys, char *, self->ob_size);
	RESIZE(entries, object *, self->ob_size);
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