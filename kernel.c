#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ram.h"

int myinit(char *filename){
    int start = 0;
    int end = 0;

    //FOR DEBUGGING PURPOSES
    char newfile[100];
    memset(newfile, '\0', 100);
    strcat(newfile, "../");
    strcat(newfile, filename);

    //WHEN NOT DEBUGGING, SET newfile to filename
    FILE *file = fopen(newfile, "rt");

    if(file == NULL){
        int errorCode = 2; // file not found error
        return errorCode;
    }

    int errorCode = addToRam(file, &start, &end);
    if(errorCode !=  0){
        return errorCode;
    }
    printRam(start, end);
    clearProgram(start, end);
    return 0;
}

