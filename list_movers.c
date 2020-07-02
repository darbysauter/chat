#include <stdio.h>
#include <stdlib.h>
#include "list.h"

extern int nextFreeList;

/*count the number of items in a list*/
int ListCount(LIST *list)
{
        /* list can not be NULL*/
        if(list == NULL){
                printf("Error in ListCount : invalid parameter list = NULL \n");
                return -1;
        }
        return list->size;
}

/* let current item be first item, and return pointer of it*/
void *ListFirst(LIST *list)
{
        /* list can not be NULL*/
        if(list == NULL){
                printf("Error in ListFirst : invalid parameter list = NULL \n");
                return NULL;
        }
        /*first node can not be NULL*/
        if((list->first) == NULL){
                printf("Error in procedure ListFirst: first item in list is Null \n");
                return NULL;
        }
        /* move current item to first item */
        list -> current = list -> first;
        return (list->current) -> data;
}

/* let current item be last item, and return pointer of it*/
void *ListLast(LIST *list)
{
        /* list can not be NULL*/
        if(list == NULL){
                printf("Error in ListLast : invalid parameter list = NULL \n");
                return NULL;
        }
        /*last node can not be NULL*/
        if(list->last == NULL){
                printf("Error in procedure ListLast: last item in list is Null \n");
                return NULL;
        }

        /* move current item to last item */
        list -> current = list -> last;
        return (list->current) -> data;
}

/* advances the list's current node by one, and return pointer of it*/
void *ListNext(LIST *list)
{
        /* list can not be NULL*/
        if(list == NULL){
                printf("Error in ListNext : invalid parameter list = NULL \n");
                return NULL;
        }
        /*current node can not be NULL*/
        if(list->current == NULL){
                printf("Error in procedure ListNext: current item in list is Null \n");
                return NULL;
        }
        else{
                if((list -> current -> next)==NULL){
                        return NULL;
                }else{
                        list -> current = list -> current -> next;
                }
        }
        return list -> current -> data;
}

/* backs up  the list's current node by one, and return pointer of it*/
void *ListPrev(LIST *list)
{
        /* list can not be NULL*/
        if(list == NULL){
                printf("Error in ListPrev : invalid parameter list = NULL \n");
                return NULL;
        }
        /*current node can not be NULL*/
        if(list -> current == NULL){
                printf("Error in procedure ListPrev: current item in list is Null \n");
                return NULL;
        }
        else{
                if((list -> current -> prev)==NULL){
                        return NULL;
                }else{
                        list -> current = list -> current -> prev;
                }
        }
        return list -> current -> data;
}

/* returns a pointer to the current item in list.*/
void *ListCurr(LIST *list)
{
        /* list can not be NULL*/
        if(list == NULL){
                printf("Error in ListCurr : invalid parameter list = NULL \n");
                return NULL;
        }
        /*current node can not be NULL*/
        if(list -> current == NULL){
                printf("Error in ListCurr: current item in list is Null \n");
                return NULL;
        }
        return (list -> current) -> data;
}

/*searches list starting at the current item until the end is reached or a match is found*/
void *ListSearch(LIST *list, int (*comparator)(Item, Item), Item comparisonArg)
{
        /* list can not be NULL*/
        if(list == NULL){
                printf("Error in ListSearch : invalid parameter list = NULL \n");
                return NULL;
        }
        /*pointer to routines can not be NULL*/
        if(comparator == NULL){
                printf("Error in ListSearch : invalid parameter comparator\n");
                return NULL;
        }
        /*current node can not be NULL*/
        if(list -> current == NULL){
                printf("Error in ListCurr: current item in list is Null \n");
                return NULL;
        }
        /*comparisonArg can not be NULL*/
        if(comparisonArg == NULL){
                printf("Error in ListCurr: comparisonArg is Null \n");
                return NULL;
        }

        if(comparator(list->current->data,comparisonArg)){
                printf("ListSearch: Found Item; current item is the item\n");
                return list->current->data;
        }else{
                while(list->current->next != NULL){
                        list->current = list->current->next;
                        if(comparator(list->current->data,comparisonArg)){
                                printf("ListSearch: Found Item by continually moving next\n");
                                return list->current->data;
                        }
                }

        }
        return NULL;
}
