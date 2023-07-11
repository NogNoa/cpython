typedef struct {
	char *key;
	object *value;
} entry;

typedef struct {
	OB_VARHEAD
	entry *ob_item;
} dictobject;


static void
dict_dealloc(op)
	dictobject *op;
{
	int i;
	for (i = 0; i < op->ob_size; i++) {
		if (op->ob_item[i] != NULL)
			free(op->ob_item[i]->key)
			DECREF(op->ob_item[i]->value);
			free(op->ob_item[i])
	}
	if (op->ob_item != NULL)
		free((ANY *)op->ob_item);
	free((ANY *)op);
}

typeobject Dicttype = {
	OB_HEAD_INIT(&Typetype)
	0, /* Number of items for varobject */
	"Dictionary",
	sizeof(dictobject),
	0, /* Item size for varobject */
	dict_dealloc,	/*tp_dealloc*/
	dict_print,	/*tp_print*/
	dict_getattr,	/*tp_getattr*/
	NULL,		/*tp_setattr*/
	dict_compare,	/*tp_compare*/
	dict_repr,	/*tp_repr*/
	NULL,		/*tp_as_number*/
	NULL,	/*tp_as_sequence*/
	&dict_as_mapping,		/*tp_as_mapping*/
};