#include <stdio.h>
#include <stdlib.h>
#include "list.h"

extern NODE *nextFreeNode;
extern LIST *nextFreeList;

/*remove the current node, and return the pointer of it*/
void *ListRemove(LIST *list)
{
        /*for saving the current item*/
        void *removedCurrentItem;
        NODE *nextTemp = NULL;
        if(list == NULL){
                printf("Error in ListRemove : invalid parameter list = NULL \n");
                return NULL;
        }
        else if(list->current == NULL){
                printf("Error in ListRemove : current item is NULL \n");
                return NULL;
        }
        else{
                removedCurrentItem = list->current->data;
                /*if current is the last node*/
                if(list->current->next == NULL){
                        if(list->current->prev != NULL){
                                list->current->prev->next = NULL;
                                nextTemp = list->current->prev;
                                list->current->prev = NULL;
                        }else{
                                list->first = NULL;
                        }
                        list->last = nextTemp;
                }
                /*if current is the first node*/
                else if(list->current->prev == NULL){
                        if(list->current->next != NULL){
                                list->current->next->prev = NULL;
                                nextTemp = list->current->next;
                                list->current->next = NULL;
                        }else{
                                list->last = NULL;
                        }
                        list->first = nextTemp;
                }
                else{
                        list->current->prev->next = list->current->next;
                        list->current->next->prev = list->current->prev;
                        nextTemp = list->current->next;
                        list->current->prev = NULL;
                        list->current->next = NULL;
                }
                list->current->data = NULL;
                list->current->occupied = 0;
                list->current->nextFreeNode = nextFreeNode;
                nextFreeNode = list->current;
                list->current = nextTemp;
                list->size -= 1;

        }
        return removedCurrentItem;
}

/*delete list, free all items in it*/
void ListFree(LIST *list, void (*itemFree)(Item))
{
        if(list == NULL){
                printf("Error in ListFree : invalid parameter list = NULL \n");
        }
        else if(itemFree == NULL){
                printf("Error in ListFree : invalid parameter itemFree = NULL \n");
        }
        else{
                /* free the list*/
                list->current = list->first;
                while(list->current != NULL){
                        itemFree(list->current->data);
                        list->current->data = NULL;
                        list->current->prev = NULL;
                        list->current->occupied = 0;
                        list->current->nextFreeNode = nextFreeNode;
                        nextFreeNode = list->current;
                        list->current = list->current->next;
                        nextFreeNode->next = NULL;
                }
                list->first = NULL;
                list->last = NULL;
                list->size = 0;
                list->used = 0;
                list->nextFreeList = nextFreeList;
                nextFreeList = list;

        }
}

/*Return last item and take it out of list, */
/*the current pointer shall be the new last item in the list.*/
void *ListTrim(LIST *list)
{
        /*for saving the current last item*/
        void *removedLstItem;
        NODE *temp = NULL;

        if(list == NULL){
                printf("Error in ListTrim : invalid parameter list = NULL \n");
                return NULL;
        }
        else if(list->last == NULL){
                printf("Error in ListTrim : last item is NULL \n");
                return NULL;
        }
        else{
        /*delete*/
                removedLstItem = list->last->data;
                list->last->data = NULL;
                list->last->occupied = 0;
                list->last->nextFreeNode = nextFreeNode;
                nextFreeNode = list->last;
                list->size -= 1;
                temp = list->last;
                list->last = list->last->prev;
                list->current = list->last;
                temp->prev = NULL;
                if(list->size==0){
                        list->first = NULL;
                }

        }

        return removedLstItem;
}
