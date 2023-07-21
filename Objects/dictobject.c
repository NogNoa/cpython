#include "allobjects.h"
#include "PROTO.h"
#include "exmalloc.h"

typedef struct {
	char *key;
	object *value;
} entry;

typedef struct {
	OB_VARHEAD
	entry **ob_item;
} dictobject;


object *
newdictobject(void)
{
	dictobject *op;
	op = (dictobject *) malloc(sizeof(dictobject));
	if (op == NULL) {
		return err_nomem();
	}
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
		if (op->ob_item[i] != NULL)
			free(op->ob_item[i]->key);
			DECREF(op->ob_item[i]->value);
			free(op->ob_item[i]);
	}
	if (op->ob_item != NULL)
		free((ANY *)op->ob_item);
	free((ANY *)op);
}


object *
dictlookup(dp, key)
	object *dp; 
	char *key;
{
	int i;
	if (!is_dictobject(dp)) {
		err_badcall();
		return NULL;
	}
	dictobject* dct = (dictobject *) dp;
	for (i = 0; i < dct->ob_size; i++) {
		if (!strcmp(key, dct->ob_item[i]->key))
			return dct->ob_item[i]->value;
	}
	err_setstr(KeyError, key);
	return NULL;
}


static int
ins(self, entr)
	dictobject *self;
	entry *entr;
{
	int where;
	entry **entries;
	if (entr == NULL) {
		err_badcall();
		return -1;
	}
	entries = self->ob_item;
	RESIZE(entries, entry *, self->ob_size+1);
	if (entries == NULL) {
		err_nomem();
		return -1;
	}
	where = (int) self->ob_size;
	if (where < 0)
		where = 0;
	entries[where] = entr;
	self->ob_item = entries;
	self->ob_size++;
	return 0;
}


int
dictinsert(dp, key, item)
object *dp, *item;
char *key;
{
	int i;
	if (key == NULL || item == NULL) {
		err_badarg();
		return NULL;
	}
	return ins(dp, , key, item);
}


int
addlistitem(dp, key, item)
object *dp, *item;
char *key;
{
	entry *entr = NEW(entry, sizeof(entry *));
	if (!is_dictobject(dp)) {
		err_badcall();
		return -1;
	}
	INCREF(item);
	entr->key = key;
	entr->value = item;
	return ins((dictobject *)dp, entr);
}


extern int dictremove(dp, key)
object *dp;
char *key;
{

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
		fprintf(fp, "%s: ", op->ob_item[i]->key);
		printobject(op->ob_item[i]->value, fp, flags);
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
		k = newstringobject(op->ob_item[i]->key);
		v = reprobject(op->ob_item[i]->value);
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