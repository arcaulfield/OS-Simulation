#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *ram[1000];
char *programName[3];
int startPtr[3] = {0, 0, 0};
int endPtr[3] = {0, 0, 0};
int maxLineLength = 1000;
int maxNameLength = 100;

//initializes RAM
void initRam(){
    for(int i = 0; i < 1000; i++){
        ram[i] = NULL;
    }
    for (int i = 0; i < 3; i++){
        programName[i] = NULL;
    }
}

//loads Program
int loadProgram(char * filename){
    int programNum = 0;
    int space = 0;
    int errorCode = 0;
    for(int i = 0; i < 3; i++){
        if(programName[i] == NULL){
            programName[i] = (char*) malloc(100 * sizeof(char));
            memset(programName[i], '\0', 100);
            strcpy(programName[i], filename);
            programNum = i;
            space = 1;
            break;
        }
    }
    if(space == 0){
        return 3;  // memory is full and there was a mistake. return error code - already 3 programs in memory
    }

    char buffer[1000];
    //indicates where we are in ram
    int k = 0;
    if(programNum != 0){
        //start the program at the next free line
        startPtr[programNum] = endPtr[programNum - 1] + 1;
        k = startPtr[programNum];
    }

    FILE *file = fopen(filename, "rt");
    if(file == NULL){
        errorCode = 2; // file not found error
        return errorCode;
    }

    fgets(buffer, 999, file);
    while(!feof(file)){
        ram[k] = (char *) malloc(1000 * sizeof(char));
        ram[k] = strdup(buffer);
        k++;
        if(k > 999 || ram[k] != NULL){
            errorCode = 4; //not enough space in ram for the program
            fclose(file);
            return errorCode;
        }
        fgets(buffer, 999, file);
    }

    endPtr[programNum] = k - 1;
    fclose(file);
    return errorCode;
}

//print function added for debugging purposes
void printRam(){
    printf("FIRST PROGRAM: %s\n", programName[0]);
    printf("start pointer: %d; end pointer: %d\n", startPtr[0], endPtr[0]);
    for(int i = startPtr[0]; i <= endPtr[0]; i++){
        printf("%s", ram[i]);
    }
    printf("SECOND PROGRAM: %s\n", programName[1]);
    printf("start pointer: %d; end pointer: %d\n", startPtr[1], endPtr[1]);
    for(int i = startPtr[1]; i <= endPtr[1]; i++){
        printf("%s", ram[i]);
    }
}


void finishExecuting(char* filename){
    int k = 0;
    for(int i = 0; i < 3; i ++){
        if(strcmp(filename, programName[i]) == 0){
            strcpy(programName[i], NULL);
            free(programName[i]);
            programName[i] = NULL;
            k = i;
        }
    }
    for(int i = startPtr[k]; i <= endPtr[k]; i ++){
        free(ram[i]);
        ram[i] = NULL;

        //FIX START AND END POINTERS
    }
    if(k == 0){
        startPtr[k] = 0;
        endPtr[k] = 0;
    }else{
        startPtr[k] = endPtr[k - 1] + 1;
        endPtr[k] = endPtr[k - 1];
    }
}

//clears all the memory from ram
//called when program finishes running
void clearRam(){
    for(int i = 0; i < 1000; i ++){
        free(ram[i]);
    }
}

