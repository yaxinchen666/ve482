//
// Created by cyx on 19-11-5.
//

#include <memory.h>
#include <stdlib.h>
#include "list_t.h"
#include "logic.h"
#include "lab5_dlist.h"

static dlistValueType constValueType;

dlist createDlist(dlistValueType type)
// EFFECTS: allocate and create a new dlist of datatype 'type' object
// returns the created dlist object on success
// returns NULL on error
{
    list_t *list = init();
    constValueType = type;
    return (dlist)list;
}

int dlistIsEmpty(dlist_const this)
// REQUIRES: argument 'this' is non-null and is a valid list
// EFFECTS: return whether the list 'this' is empty
{
    list_t *list = (list_t *) this;
    return (list->head == NULL);
}

void dlistAppend(dlist this, const char* key, dlistValue value)
// REQUIRES: type instantiated in 'value' corresponds to the datatype in the list
// argument 'this' and 'key' are both non-null and valid lists
// EFFECTS : add a line of form "key=value" to the tail of the dlist
// MODIFIES: modifies 'this'.
{
    //node_t *createnode(char *name, void *value, int datatype, size_t size)
    //void insert(list_t *list, node_t *node)
    /*
     * DLIST_INT = 0x01, // list contains int values
    DLIST_STR = 0x02, // list contains char* values
    DLIST_DOUBLE = 0x03, // list contains double values
    DLIST_UNKOWN = 0x00

     if(datatype == INT){
                int tvalue = atoi(value);
                node_t *temp = createnode(name, &tvalue, INT, sizeof(int));
                insert(list, temp);
            }
            else if(datatype == DOUBLE){
                double tvalue = atof(value);
                node_t *temp = createnode(name, &tvalue, DOUBLE, sizeof(double));
                insert(list, temp);
            }
            else{
                node_t *temp = createnode(name, &value, CHAR, (valueindex+1)*sizeof(char));
                insert(list, temp);
            }
     */
    list_t *list = (list_t *)this;
    node_t *node;
    if(constValueType == DLIST_INT){
        int tvalue = value.intValue;
        node = createnode(key, &tvalue, INT, sizeof(int));
    }
    else if(constValueType == DLIST_DOUBLE){
        double tvalue = value.doubleValue;
        node = createnode(key, &tvalue, DOUBLE, sizeof(double));
    }
    else if(constValueType == DLIST_STR){ //TODO
        node = createnode(key, &(value.strValue), CHAR, (strlen(value.strValue)+1)* sizeof(char));
    }
    else{
        fprintf(stderr, "In correct ValueType.\n");
        exit(1);
    }
    insert(list, node);
}

enum DataType_t getlistdatatype(dlistValueType ValueType)
{
    if(ValueType == DLIST_INT) return INT;
    else if(ValueType == DLIST_DOUBLE) return DOUBLE;
    else if(ValueType == DLIST_STR) return CHAR;
    else{
        fprintf(stderr, "In correct ValueType.\n");
        exit(1);
    }
}

node_t *copynode(node_t *onode, int datatype)
{
    node_t *newnode;
    if(datatype == INT){
        newnode = createnode(onode->name, onode->value, datatype, sizeof(int));
    }
    else if(datatype == DOUBLE){
        newnode = createnode(onode->name, onode->value, datatype, sizeof(double));
    }
    else{

        newnode = createnode(onode->name, onode->value, datatype, (strlen((char *)onode->value)+1)*sizeof(char));
    }

    return newnode;
}

void dlistSort(dlist_const this, dlist listDst, dlistSortMethod method)
// REQUIRES: argument 'this' is not null and is a valid list
// argument 'listDst' is not null and is a valid list
// EFFECTS: Sort 'listSrc' using method given by 'method'.
// Put the results in 'listDst' on success
// Leave 'listDst' unchanged on failure
// * Note 'listDst' may be non-empty when invoked.
// * Be very careful with memory management regarding string lists.
// MODIFIES: argument 'listDst'
{
    //logic_sorting(list_t *list, enum DataType_t datatype, enum SortingType_t sortingtype)
    if(method == DLIST_SORT_UNKOWN) return;

    list_t *oldlist = (list_t *)this;
    list_t *newlist = (list_t *)listDst;

    if(newlist!=NULL){
        del(newlist);
        newlist = init();
    }
    enum DataType_t list_t_datatype = getlistdatatype(constValueType);
    for(node_t *node=oldlist->head; node != NULL; node = node->next){
        node_t *newnode = copynode(node, list_t_datatype);
        insert(newlist, newnode);
    }

    if(method == DLIST_SORT_INC){
        logic_sorting(newlist, list_t_datatype, INC);
    }
    else if(method == DLIST_SORT_DEC){
        logic_sorting(newlist, list_t_datatype, DEC);
    }
    else{
        logic_sorting(newlist, list_t_datatype, RAND);
    }
}

void dlistPrint(dlist_const this)
// REQUIRES: argument 'this' is not null and is a valid list
// EFFECTS: Print the content of 'this' in the required format to standard output.
// A newline must be followed after each line (including the last one).
{
    list_t *list = (list_t *)this;
    enum DataType_t dataType = getlistdatatype(constValueType);
    print(list, dataType, stdout);
}

void dlistFree(dlist this)
// REQUIRES: argument 'this' is <EITHER> NULL or a valid list
// EFFECTS: if 'this' is NULL do nothing, other wise frees the list
// By freeing the list the user also needs to free the data
// the list manages
{
    if(this != NULL){
        list_t *list = (list_t *)this;
        del(list);
    }
}
