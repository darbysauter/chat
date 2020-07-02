#include <stdio.h>
#include <stdlib.h>
#include "list.h"

LIST *lpool = NULL;
NODE *npool = NULL;

LIST *nextFreeList = NULL;
NODE *nextFreeNode = NULL;

/*Extra credit portion: for calculating the new memory space reallocated*/
int exponent = 1;
int listsExponent = 1;

/*create a new list and return its pointer*/
LIST *ListCreate()
{
        int i = 0;
        int j = 0;
        LIST* newListPoint;
        LIST *lpooltemp = NULL;
        int listsExponentTemp = listsExponent;
        int base = 2;
        int times = 1;
        int timesOld = 1;
        int newSize;
        int oldSize;


        /*list pool creating*/
        if(lpool == NULL){
                lpool = (LIST *)calloc(MIN_LISTS,sizeof(LIST));
                if(lpool==NULL){
                        return NULL;
                }else{
                        nextFreeList = lpool;
                        while(i < MIN_LISTS-1){
                                lpool[i].nextFreeList = &lpool[i+1];
                                i++;
                        }
                }
        }
        /*node pool creating*/
        if(npool == NULL){
                npool = (NODE *)calloc(MIN_NODES,sizeof(NODE));
                if(npool==NULL){
                        return NULL;
                }else{
                        nextFreeNode = npool;
                        while(j < MIN_NODES-1){
                                npool[j].nextFreeNode = &npool[j+1];
                                j++;
                        }
                }
        }

        /*Extra credit portion: calculate the new memory space reallocated*/
        while(listsExponentTemp!=0){
                times *= base;
                listsExponentTemp--;
        }
        listsExponentTemp = listsExponent-1;
        while(listsExponentTemp!=0){
                timesOld *= base;
                listsExponentTemp--;
        }
        newSize = (int)(times * MIN_LISTS);
        oldSize = (int)(timesOld * MIN_LISTS);

        /*Extra credit portion: reallocate space*/
        if(nextFreeList == NULL){
                printf("Lists Space is not enough. double the space\n");
                if(newSize > LISTMAXIMUM){
                        printf("No more space can be reallocated to new list\n");
                        return NULL;
                }
                lpooltemp = lpool;
                lpool = (LIST *)realloc(lpool,newSize * sizeof(LIST));
                printf("oldSize: %d\n",oldSize);
                printf("newSize: %d\n",newSize);
                if(lpool == NULL){
                        printf("reallocate unsuccessfully\n");
                        lpool = lpooltemp;
                        return NULL;
                }else{
                        nextFreeList = &lpool[oldSize];
                        while(oldSize < newSize-1){
                                lpool[oldSize].nextFreeList = &lpool[oldSize+1];
                                oldSize++;
                        }
                        listsExponent += 1;
                }
        }

        /*new empty list created*/
        newListPoint = nextFreeList;
        newListPoint->size=0;
        newListPoint->first=NULL;
        newListPoint->last=NULL;
        newListPoint->current=NULL;
        newListPoint->used = 1;
        nextFreeList = nextFreeList -> nextFreeList;
        newListPoint -> nextFreeList = NULL;

        return newListPoint;
}

/* add the Item after the current item of the list */
int ListAdd(LIST *list, Item item)
{
        NODE* newNodePoint = NULL;
        NODE* oldNextNodePoint;
        int oldSize;
        int newSize;
        int i;
        NODE *npooltemp = NULL;

        int exponentTemp = exponent;
        int base = 2;
        int times = 1;
        int timesOld = 1;

        /*Extra credit portion: calculate the new memory space reallocated*/
        while(exponentTemp!=0){
                times *= base;
                exponentTemp--;
        }
        exponentTemp = exponent-1;
        while(exponentTemp!=0){
                timesOld *= base;
                exponentTemp--;
        }
        newSize = (int)(times * MIN_NODES);
        oldSize = (int)(timesOld* MIN_NODES);
        i = oldSize;

        /* list and current pointer cannot be NULL*/
        if(npool == NULL){
                printf("Error in ListAdd : node pool is empty \n");
                return -1;
        }
        if(list == NULL){
                printf("Error in ListAdd : invalid parameter list = NULL \n");
                return -1;
        }
        if(list -> current == NULL && list -> size > 0){
                printf("Error in ListAdd : non-empty list with NULL current \n");
                return -1;
        }
        /*lists should all be created and used by ListCreate*/
        if(!(list -> used)){
                printf("Error in ListAdd: illegal unused list\n");
                return -1;
        }
        /*Extra credit portion: reallocate space*/
        if(nextFreeNode == NULL){
                printf("Space is not enough. double the space\n");
                if(newSize > NODEMAXIMUM){
                        printf("No more space can be reallocated\n");
                        return -1;
                }
                npooltemp = npool;
                npool = (NODE *)realloc(npool,newSize * sizeof(NODE));
                printf("oldSize: %d\n",oldSize);
                printf("newSize: %d\n",newSize);
                if(npool == NULL){
                        printf("reallocate unsuccessfully\n");
                        npool = npooltemp;
                        return -1;
                }else{
                        nextFreeNode = &npool[oldSize];
                        while(i < newSize-1){
                                npool[i].nextFreeNode = &npool[i+1];
                                i++;
                        }
                        exponent += 1;
                }
        }

        newNodePoint = nextFreeNode;
        if(list -> current == NULL && list -> size == 0){
                newNodePoint -> data = item;
                newNodePoint -> next = NULL;
                newNodePoint -> prev = NULL;
                newNodePoint -> occupied = 1;
                list -> size += 1;
                nextFreeNode = nextFreeNode -> nextFreeNode;
                newNodePoint -> nextFreeNode = NULL;
                list -> first = newNodePoint;
                list -> last = newNodePoint;
                list -> current = newNodePoint;
        }
        else{
                newNodePoint -> data = item;
                newNodePoint -> occupied = 1;
                list -> size += 1;
                oldNextNodePoint = list->current->next;
                list->current->next = newNodePoint;
                newNodePoint->prev = list->current;
                newNodePoint->next = oldNextNodePoint;
                nextFreeNode = nextFreeNode -> nextFreeNode;
                newNodePoint -> nextFreeNode = NULL;
                list -> current = newNodePoint;
                if(oldNextNodePoint==NULL){
                        list -> last = newNodePoint;
                        return 0;
                }
                oldNextNodePoint->prev = newNodePoint;
        }
        return 0;
}


/* add the Item before the current item of the list*/
int ListInsert(LIST *list, Item item)
{
        /* list and current pointer cannot be NULL*/
        NODE* newNodePoint;
        NODE* oldPrevNodePoint = NULL;
        int oldSize;
        int newSize;
        int i;
        NODE *npooltemp = NULL;

        int exponentTemp = exponent;
        int base = 2;
        int times = 1;
        int timesOld = 1;

        /*Extra credit portion: calculate the new memory space reallocated*/
        while(exponentTemp!=0){
                times *= base;
                exponentTemp--;
        }
        exponentTemp = exponent-1;
        while(exponentTemp!=0){
                timesOld *= base;
                exponentTemp--;
        }
        newSize = (int)(times * MIN_NODES);
        oldSize = (int)(timesOld* MIN_NODES);
        i = oldSize;

        if(list == NULL){
                printf("Error in ListInsert : invalid parameter list = NULL \n");
                return -1;
        }
        if(npool == NULL){
                printf("Error in ListInsert : node pool is empty \n");
                return -1;
        }
        /*lists should all be created and used by ListCreate*/
        if(!(list -> used)){
                printf("Error in ListInsert: illegal unused list\n");
                return -1;
        }
        /*Extra credit portion: reallocate space*/
        if(nextFreeNode == NULL){
                printf("Space is not enough. double the space\n");
                if(newSize > NODEMAXIMUM){
                        printf("No more space can be reallocated\n");
                        return -1;
                }
                npooltemp = npool;
                npool = (NODE *)realloc(npool,newSize * sizeof(NODE));
                printf("oldSize: %d\n",oldSize);
                printf("newSize: %d\n",newSize);
                if(npool == NULL){
                        printf("reallocate unsuccessfully\n");
                        npool = npooltemp;
                        return -1;
                }else{
                        nextFreeNode = &npool[oldSize];
                        while(i < newSize-1){
                                npool[i].nextFreeNode = &npool[i+1];
                                i++;
                        }
                        exponent += 1;
                }
        }
        newNodePoint = nextFreeNode;
        if(list -> current == NULL && list -> size == 0){
                newNodePoint -> data = item;
                newNodePoint -> next = NULL;
                newNodePoint -> prev = NULL;
                newNodePoint -> occupied = 1;
                list -> size += 1;
                nextFreeNode = nextFreeNode -> nextFreeNode;
                newNodePoint -> nextFreeNode = NULL;
                list -> first = newNodePoint;
                list -> last = newNodePoint;
                list -> current = newNodePoint;
        }else{
                newNodePoint -> data = item;
                newNodePoint -> occupied = 1;
                list -> size += 1;
                oldPrevNodePoint = list->current->prev;
                list->current->prev = newNodePoint;
                newNodePoint->next = list->current;
                newNodePoint->prev = oldPrevNodePoint;
                nextFreeNode = nextFreeNode -> nextFreeNode;
                newNodePoint -> nextFreeNode = NULL;
                list -> current = newNodePoint;
                if(oldPrevNodePoint==NULL){
                        list -> first = newNodePoint;
                        return 0;
                }
                oldPrevNodePoint->next = newNodePoint;
        }
        return 0;
}

/*adds item to the end of list*/
int ListAppend(LIST *list, Item item)
{
        NODE* newNodePoint;
        int oldSize;
        int newSize;
        int i;
        NODE *npooltemp = NULL;

        int exponentTemp = exponent;
        int base = 2;
        int times = 1;
        int timesOld = 1;

        /*Extra credit portion: calculate the new memory space reallocated*/
        while(exponentTemp!=0){
                times *= base;
                exponentTemp--;
        }
        exponentTemp = exponent-1;
        while(exponentTemp!=0){
                timesOld *= base;
                exponentTemp--;
        }
        newSize = (int)(times * MIN_NODES);
        oldSize = (int)(timesOld* MIN_NODES);
        i = oldSize;

        /* list pointer can not be NULL*/
        if(list == NULL){
                printf("Error in ListAppend : invalid parameter list = NULL \n");
                return -1;
        }
        if(npool == NULL){
                printf("Error in ListAppend : node pool is empty \n");
                return -1;
        }
        /*lists should all be created and used by ListCreate*/
        if(!(list -> used)){
                printf("Error in ListAppend: illegal unused list\n");
                return -1;
        }
        /*Extra credit portion: reallocate space*/
        if(nextFreeNode == NULL){
                printf("Space is not enough. double the space\n");
                if(newSize > NODEMAXIMUM){
                        printf("No more space can be reallocated\n");
                        return -1;
                }
                npooltemp = npool;
                npool = (NODE *)realloc(npool,newSize * sizeof(NODE));
                printf("oldSize: %d\n",oldSize);
                printf("newSize: %d\n",newSize);
                if(npool == NULL){
                        printf("reallocate unsuccessfully\n");
                        npool = npooltemp;
                        return -1;
                }else{
                        nextFreeNode = &npool[oldSize];
                        while(i < newSize-1){
                                npool[i].nextFreeNode = &npool[i+1];
                                i++;
                        }
                        exponent += 1;
                }
        }
        newNodePoint = nextFreeNode;
        if(list -> current == NULL && list -> size == 0){
                newNodePoint -> data = item;
                newNodePoint -> next = NULL;
                newNodePoint -> prev = NULL;
                newNodePoint -> occupied = 1;
                list -> first = newNodePoint;
        }else{
                newNodePoint -> data = item;
                newNodePoint -> occupied = 1;
                newNodePoint -> next = NULL;
                newNodePoint -> prev = list->last;
                list->last->next = newNodePoint;

        }
        list -> size += 1;
        list->last = newNodePoint;
        list -> current = newNodePoint;
        nextFreeNode = nextFreeNode -> nextFreeNode;
        newNodePoint -> nextFreeNode = NULL;
        return 0;
}

/*adds item to the front of list*/
int ListPrepend(LIST *list, Item item)
{
        NODE* newNodePoint;
        int oldSize;
        int newSize;
        int i;
        NODE *npooltemp = NULL;

        int exponentTemp = exponent;
        int base = 2;
        int times = 1;
        int timesOld = 1;

        /*Extra credit portion: calculate the new memory space reallocated*/
        while(exponentTemp!=0){
                times *= base;
                exponentTemp--;
        }
        exponentTemp = exponent-1;
        while(exponentTemp!=0){
                timesOld *= base;
                exponentTemp--;
        }
        newSize = (int)(times * MIN_NODES);
        oldSize = (int)(timesOld* MIN_NODES);
        i = oldSize;
        /* list pointer can not be NULL*/
        if(list == NULL){
                printf("Error in ListPrepend : invalid parameter list = NULL \n");
                return -1;
        }
        if(npool == NULL){
                printf("Error in ListPrepend : node pool is empty \n");
                return -1;
        }
        /*lists should all be created and used by ListCreate*/
        if(!(list -> used)){
                printf("Error in ListPrepend: illegal unused list\n");
                return -1;
        }
        /*Extra credit portion: reallocate space*/
        if(nextFreeNode == NULL){
                printf("Space is not enough. double the space\n");
                if(newSize > NODEMAXIMUM){
                        printf("No more space can be reallocated\n");
                        return -1;
                }
                npooltemp = npool;
                npool = (NODE *)realloc(npool,newSize * sizeof(NODE));
                printf("oldSize: %d\n",oldSize);
                printf("newSize: %d\n",newSize);
                if(npool == NULL){
                        printf("reallocate unsuccessfully\n");
                        npool = npooltemp;
                        return -1;
                }else{
                        nextFreeNode = &npool[oldSize];
                        while(i < newSize-1){
                                npool[i].nextFreeNode = &npool[i+1];
                                i++;
                        }
                        exponent += 1;
                }
        }
        newNodePoint = nextFreeNode;
        if(list -> current == NULL && list -> size == 0){
                newNodePoint -> data = item;
                newNodePoint -> next = NULL;
                newNodePoint -> prev = NULL;
                newNodePoint -> occupied = 1;
                list -> last = newNodePoint;
        }else{
                newNodePoint -> data = item;
                newNodePoint -> occupied = 1;
                newNodePoint -> next = list->first;
                newNodePoint -> prev = NULL;
                list->first->prev = newNodePoint;
        }
        list -> size += 1;
        list->first = newNodePoint;
        list -> current = newNodePoint;
        nextFreeNode = nextFreeNode -> nextFreeNode;
        newNodePoint -> nextFreeNode = NULL;
        return 0;
}

/*adds list2 to the end of list1*/
void ListConcat(LIST *list1,LIST *list2)
{

        /* both lists' pointer can not be NULL*/
        if(list1 == NULL || list2 == NULL){
                printf("Error in ListConcat : invalid parameter list = NULL \n");
                return;
        }
        /*lists should all be created and used by ListCreate*/
        if(!(list1 -> used) || !(list2 -> used)){
                printf("Error in ListConcat: illegal unused list\n");
                return;
        }

        /*list1 and list2 are not empty, add all list2 nodes to list1*/
        if(list1 -> size > 0 && list2 -> size > 0){
                        /* connect 2 lists*/
                list1->last->next = list2->first;
                list2->first->prev = list1->last;
                list1->last = list2->last;
                list1->size = (list1->size) + (list2->size);
        }
        /*list1 is empty, copy all list2 to list1*/
        if(list1 -> size == 0 && list2 -> size > 0){
                list1->size = list2->size;
                list1->first = list2->first;
                list1->last = list2->last;
                list1->current = list2->current;
        }
        /* free list2 */
        list2->size = 0;
        list2->first = NULL;
        list2->last  = NULL;
        list2->current = NULL;
        list2->used = 0;
        list2->nextFreeList = nextFreeList;
        nextFreeList = list2;
}
