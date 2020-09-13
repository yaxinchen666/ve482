//
// Created by cyx on 19-11-4.
//

#include <stdio.h>
#include <memory.h>
#include "logic.h"
#include "interface.h"

int Commadline(int argc, char *argv[])
/*
 * Command line user interface
 */
{
    enum SortingType_t originalsortingtype = getoriginalsortingtype(argv[1]);
    enum DataType_t datatype = getdatatype(argv[1]);
    enum SortingType_t sortingtype = CtoST(argv[2]);


    read_sort(originalsortingtype, datatype, sortingtype);

    return 0;
}

int Menu(int argc, char *argv[])
/*
 * Menu user interface
 */
{
    printf("Welcome!\n");

    while(1){
        char char_datatype[10];
        char char_originalsortingtype[10];
        char char_sortingtype[10];

        printf("Please input one of the following initial sorting types:\n");
        printf("inc\n");
        printf("dec\n");
        printf("rand\n");
        fflush(stdout);
        fgets(char_originalsortingtype, 10, stdin);
        char_originalsortingtype[strlen(char_originalsortingtype)-1]='\0';

        printf("Please input one of the following data types:\n");
        printf("int\n");
        printf("double\n");
        printf("char*\n");
        fflush(stdout);
        fgets(char_datatype, 10, stdin);
        char_datatype[strlen(char_datatype)-1]='\0';

        printf("Please input one of the following sorting type you want to use:\n");
        printf("inc\n");
        printf("dec\n");
        printf("rand\n");
        fflush(stdout);
        fgets(char_sortingtype, 10, stdin);
        char_sortingtype[strlen(char_sortingtype)-1]='\0';

        enum SortingType_t originalsortingtype = CtoST(char_originalsortingtype);
        enum DataType_t datatype = CtoDT(char_datatype);
        enum SortingType_t sortingtype = CtoST(char_sortingtype);

        if(read_sort(originalsortingtype, datatype, sortingtype)==0){
            printf("Success!\n");
            fflush(stdout);
        }

        printf("Do you want to exit?\n");
        printf("Yes\n");
        printf("No\n");
        fflush(stdout);
        char exit[5];
        fgets(exit, 5, stdin);

        if(strcmp(exit, "Yes\n") == 0) break;

    }

    return 0;
}
