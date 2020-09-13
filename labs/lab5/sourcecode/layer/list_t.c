//
// Created by cyx on 19-10-10.
//

#include <malloc.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include "list_t.h"

list_t *init()
/*
 * EFFECTS: initialize a new list
 */
{
    list_t *list = malloc(sizeof(list_t));
    list->head = NULL;
    list->tail = NULL;
    return list;
}

void del(list_t *list)
/*
 * EFFECTS: delete a list
 */
{
    node_t *node = list->head;
    while(node != NULL){
        node_t *temp = node;
        node = node->next;
        free(temp->value);
        free(temp->name);
        free(temp);
    }
    free(list);
}

node_t *createnode(char *name, void *value, int datatype, size_t size)
/*
 * EFFECTS: create a node with name='name', value='value';
 *          'datatype' could be int, double and char*; 'size' is the size of the 'value';
 */
{
    node_t *node = malloc(sizeof(node_t));

    node->name = malloc((strlen(name)+1)*sizeof(char));
    int i;
    for(i = 0; i < strlen(name); i++){
        node->name[i] = name[i];
    }
    node->name[i] = '\0';
    if(datatype == INT){
        node->value = malloc(sizeof(int));
        memcpy(node->value, value, size);
    }
    else if(datatype == DOUBLE){
        node->value = malloc(sizeof(double));
        memcpy(node->value, value, size);
    }
    else{
        node->value = malloc(size);
        memcpy(node->value, value, size);
    }
    node->next = NULL;

    return node;
}

void insert(list_t *list, node_t *node)
/*
 * EFFECTS: insert a node to the list
 */
{
    if(list->head == NULL){
        list->head = node;
        list->tail = node;
    }
    else{
        list->tail->next = node;
        list->tail = node;
    }
}

node_t *swap(node_t *former, node_t *latter)
/*
 * EFFECTS: swap two nodes in the list
 */
{
    former->next = latter->next;
    latter->next = former;
    return latter;
}

void print(list_t *list, int datatype, FILE *file)
/*
 * EFFECTS: print all the nodes in the list
 */
{
    node_t *node;
    if(list == NULL || list->head == NULL) return;
    if(datatype == INT){
        for(node = list->head; node != NULL; node = node->next){
            char newline[LINELENGTH];
            sprintf(newline, "%s=%d\n", node->name, *((int *)(node->value)));
            fputs(newline, file);
        }
    }
    else if(datatype == DOUBLE){
        for(node = list->head; node != NULL; node = node->next){
            char newline[LINELENGTH];
            sprintf(newline, "%s=%f\n", node->name, *((double *)(node->value)));
            fputs(newline, file);
        }
    }
    else{
        for(node = list->head; node != NULL; node = node->next){
            char newline[LINELENGTH];
            sprintf(newline, "%s=%s\n", node->name, (char*)(node->value));
            fputs(newline, file);
        }
    }
}


void sort(list_t *list, bool (*fn)(node_t *, node_t *, int), int type)
/*
 * EFFECTS: sort the 'list' according to function pointer 'fn'
 */
{
    node_t *beforehead = malloc(sizeof(node_t));
    beforehead->name = NULL;
    beforehead->value = NULL;
    beforehead->next = list->head;

    node_t *it2, *it;
    int count=0;
    it = beforehead->next;
    while(it != NULL){
        it =it->next;
        count++;
    }
    for(int i =0; i<count; i++){
        for(it2 = beforehead; it2 != NULL; it2 = it2->next){
            if(it2->next != NULL && it2->next->next != NULL){
                bool result = fn(it2->next, it2->next->next, type);
                if(result){
                    it2->next = swap(it2->next, it2->next->next);
                }
            }
        }
    }

    list->head = beforehead->next;
    free(beforehead);
}

bool search(list_t *list, void *value, int datatype)
/*
 * EFFECTS: search whether 'value' is contained in the list
 */
{
    node_t *node;
    if(list == NULL || list->head == NULL) return false;
    node = list->head;
    while(node != NULL){
        if(datatype == INT){
            if(*((int *)(node->value)) == *((int *)(value))){
                return true;
            }
        }
        else if(datatype == DOUBLE){
            if(*((double *)(node->value)) == *((double *)(value))){
                return true;
            }
        }
        else{
            if(strcmp((char*)(node->value), (char*)(value)) == 0){
                return true;
            }
        }
        node = node->next;
    }
    return false;
}
