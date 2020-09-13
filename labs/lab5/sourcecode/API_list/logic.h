//
// Created by cyx on 19-11-5.
//

#ifndef API_LIST_LOGIC_H
#define API_LIST_LOGIC_H

#define NAMELENGTH 32
#define VALUELENGTH 32

#include "list_t.h"


enum SortingType_t {RAND, INC, DEC};

bool inc(node_t *a, node_t *b, int type);

bool dec(node_t *a, node_t *b, int type);

bool randtype(node_t *a, node_t *b, int type);

enum SortingType_t CtoST(char *argv);

enum DataType_t CtoDT(char *argv);

enum DataType_t getdatatype(char* argv);

enum SortingType_t getoriginalsortingtype(char *argv);

int logic_sorting(list_t *list, enum DataType_t datatype, enum SortingType_t sortingtype);

int read_sort(enum SortingType_t originalsortingtype, enum DataType_t datatype, enum SortingType_t sortingtype);


#endif //API_LIST_LOGIC_H
