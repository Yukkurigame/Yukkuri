#ifndef __LIST_H
#define __LIST_H

typedef struct
{
	int item_count;
	int current_max_size;
	char growable;

	void **items;
	char **names;	
} objl_list;

void list_make(objl_list *listo, int size, char growable);
int list_add_item(objl_list *listo, void *item, char *name);
char* list_print_items(objl_list *listo);
void* list_get_name(objl_list *listo, char *name);
void* list_get_index(objl_list *listo, int indx);
void* list_get_item(objl_list *listo, void *item_to_find);
int list_find(objl_list *listo, char *name_to_find);
void list_delete_index(objl_list *listo, int indx);
void list_delete_name(objl_list *listo, char *name);
void list_delete_item(objl_list *listo, void *item);
void list_delete_all(objl_list *listo);
void list_print_list(objl_list *listo);
void list_free(objl_list *listo);

void test_list();
#endif
