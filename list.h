#ifndef _LIST_
#define _LIST_


/* define maximum number for nodes and lists*/
#define Item void*
#define LISTMAXIMUM 1280
#define NODEMAXIMUM 1280

/*extra credit: minimum allocation for pools*/
#define MIN_LISTS 5
#define MIN_NODES 5

/* define NODE*/
typedef struct node
{
        void *data;
    struct node *next;
    struct node *prev;
    struct node *nextFreeNode;
    int occupied;
} NODE;

/* define LIST*/
typedef struct list
{
        int size;
        struct node *first;
        struct node *last;
        struct node *current;
        struct list *nextFreeList;
        int used;
} LIST;

/*function prototype*/
LIST *ListCreate();
int ListCount(LIST *list);
void *ListFirst(LIST *list);
void *ListLast(LIST *list);
void *ListNext(LIST *list);
void *ListPrev(LIST *list);
void *ListCurr(LIST *list);
int ListAdd(LIST *list, Item item);
int ListInsert(LIST *list, Item item);
int ListAppend(LIST *list, Item item);
int ListPrepend(LIST *list, Item item);
void *ListRemove(LIST *list);
void ListConcat(LIST *list1,LIST *list2);
void ListFree(LIST *list, void (*itemFree)(Item));
void *ListTrim(LIST *list);
void *ListSearch(LIST *list, int (*comparator)(Item, Item), Item comparisonArg);

#endif
