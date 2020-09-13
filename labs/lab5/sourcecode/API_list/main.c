#include <stdio.h>
#include <stdlib.h>
#include "lab5_dlist.h"

int main() {
    dlist mylist = createDlist(DLIST_INT);
    for(int i=0; i<5; i++){
        dlistValue myvalue;
        myvalue.intValue=rand()%40;
        dlistAppend(mylist, "aaa", myvalue);
    }
    dlistPrint(mylist);
    printf("Is my list empty? %d\n", dlistIsEmpty(mylist));
    dlist mynewlist = createDlist(DLIST_INT);
    dlistSort(mylist, mynewlist, DLIST_SORT_DEC);
    dlistPrint(mynewlist);
    dlistFree(mylist);
    dlistFree(mynewlist);
    return 0;
}