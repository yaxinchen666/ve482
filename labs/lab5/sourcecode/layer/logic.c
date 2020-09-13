//
// Created by cyx on 19-11-4.
//


#include <stdbool.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include "list_t.h"
#include "logic.h"

char *sortingtypename[3] = {"rand", "inc", "dec"};
char *datatypename[3] = {"char*", "int", "double"};

bool inc(node_t *a, node_t *b, int type)
/*
 * EFFECTS: return true if the value of node 'a' is larger than the value of node 'b'; return false otherwise.
 */
{
    if(type == CHAR){
        if(strcmp((char *)(a->value), (char *)(b->value)) > 0) return true;
        else return false;
    }
    else if(type == INT) return (*((int *)(a->value))) > (*((int *)(b->value)));
    else return (*((double *)(a->value))) > (*((double *)(b->value)));
}

bool dec(node_t *a, node_t *b, int type)
/*
 * EFFECTS: return true if the value of node 'a' is smaller than the value of node 'b'; return false otherwise.
 */
{
    if(type == CHAR){
        if(strcmp((char *)(a->value), (char *)(b->value)) < 0) return true;
        else return false;
    }
    else if(type == INT) return (*((int *)(a->value))) < (*((int *)(b->value)));
    else return (*((double *)(a->value))) < (*((double *)(b->value)));
}

bool randtype(node_t *a, node_t *b, int type)
/*
 * EFFECTS: return true and false randomly
 */
{
    return (rand()%10 < 5);
}

enum SortingType_t CtoST(char *argv)
/*
 * EFFECTS: convert 'argv' to according sortingtype: INC/DEC/RAND
 */
{
    if(strcmp(argv, "inc") == 0) return INC;
    else if(strcmp(argv, "dec") == 0) return DEC;
    else return RAND;
}

enum DataType_t CtoDT(char *argv)
/*
 * EFFECTS: convert 'argv' to according datatype: INT/DOUBLE/CHAR
 */
{
    if(strcmp(argv, "int") == 0) return INT;
    else if(strcmp(argv, "double") == 0) return DOUBLE;
    else return CHAR;
}

enum DataType_t getdatatype(char* argv)
/*
 * EFFECTS: convert argv[1] to according datatype: INT/DOUBLE/CHAR
 */
{
    enum DataType_t datatype;
    char sortingchar[5];
    char datachar[7];
    int i = 0;
    for(i = 0; i < 4; i++){
        if(argv[i] == '_') break;
        sortingchar[i] = argv[i];
    }
    sortingchar[i] = '\0';
    i++;
    int ii;
    for(ii = 0; ii < 6; ii++){
        if(argv[i+ii] == '.') break;
        datachar[ii] = argv[i+ii];
    }
    datachar[ii] = '\0';

    datatype = CtoDT(datachar);

    return datatype;
}

enum SortingType_t getoriginalsortingtype(char *argv)
/*
 * EFFECTS: convert argv[1] to according sortingtype: INC/DEC/RAND
 */
{
    enum SortingType_t original;
    char sortingchar[5];
    int i = 0;
    for(i = 0; i < 4; i++){
        if(argv[i] == '_') break;
        sortingchar[i] = argv[i];
    }
    sortingchar[i] = '\0';
    original = CtoST(sortingchar);
    return original;
}

int logic_sorting(list_t *list, enum DataType_t datatype, enum SortingType_t sortingtype)
/*
 * EFFECTS: sort 'list' according to its datatype and sortingtype
 */
{
    // sort
    printf("sorting elements\n");
    if(datatype == INT){
        if(sortingtype == INC){
            sort(list, inc, INT);
        }
        else if(sortingtype == DEC){
            sort(list, dec, INT);
        }
        else{
            sort(list, randtype, INT);
        }
    }
    else if(datatype == DOUBLE){
        if(sortingtype == INC){
            sort(list, inc, DOUBLE);
        }
        else if(sortingtype == DEC){
            sort(list, dec, DOUBLE);
        }
        else{
            sort(list, randtype, DOUBLE);
        }
    }
    else{
        if(sortingtype == INC){
            sort(list, inc, CHAR);
        }
        else if(sortingtype == DEC){
            sort(list, dec, CHAR);
        }
        else{
            sort(list, randtype, CHAR);
        }
    }
    return 0;
}

int read_sort(enum SortingType_t originalsortingtype, enum DataType_t datatype, enum SortingType_t sortingtype)
{
    list_t *list = init();

    char filename[20];
    sprintf(filename, "%s_%s.txt", sortingtypename[originalsortingtype], datatypename[datatype]);
    printf("reading %s\n", filename);
    FILE *fd = fopen(filename, "r");

    // read file to the list
    if(fd != NULL){

        char line[LINELENGTH];
        while(fgets(line, LINELENGTH, fd) != NULL){

            char name[NAMELENGTH];
            int lineindex=0;
            while(line[lineindex] != '='){
                name[lineindex] = line[lineindex];
                lineindex++;
            }
            name[lineindex] = '\0';

            char value[VALUELENGTH];
            int valueindex = 0;
            lineindex++;
            while(line[lineindex+valueindex] != '\n'){
                value[valueindex] = line[lineindex+valueindex];
                valueindex++;
            }
            value[valueindex] = '\0';


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
        }

        fclose(fd);
    }
    else{
        printf("invalid file\n");
    }

    logic_sorting(list, datatype, sortingtype);

    // write to new file
    char newfilename[20];
    sprintf(newfilename, "%s_%s.txt", sortingtypename[sortingtype], datatypename[datatype]);

    FILE *newfd = fopen(newfilename, "w");
    printf("writing %s\n", newfilename);
    print(list, datatype, newfd);
    fclose(newfd);

    del(list);

    return 0;
}
