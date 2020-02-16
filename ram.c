#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *ram[1000];
char *programName[3];
int startPtr[3] = {0, 0, 0};
int endPtr[3] = {0, 0, 0};
int programCount = 0;

//initializes RAM
void initRam(){
    for(int i = 0; i < 1000; i++){
        ram[i] = NULL;
    }
    for (int i = 0; i < 3; i++){
        programName[i] = NULL;
    }
}

int addToRam(FILE *p, int* start, int* end){
    if(programCount >= 3){
        return 3; //too many programs in ram error code
    }
    programCount ++;
    char buffer[1000];
    fgets(buffer, 999, p);
    int k = 0;
    for(int i =0; i < 1000; i++){
        if (ram[i] == NULL){
            *start = i;
            k = i;
            break;
        }
    }

    while(!feof(p)){
        ram[k] = (char *) malloc(1000 * sizeof(char));
        ram[k] = strdup(buffer);
        k++;
        if(k > 999 || ram[k] != NULL){
            printf("WARNING: there was not enough space to load the entire program into RAM. Only part of the program was loaded.");
            *end = k - 1;
            fclose(p);
            return 0;
        }
        fgets(buffer, 999, p);
    }

    *end = k - 1;
    fclose(p);
    return 0;
}
//print function added for debugging purposes
void printRam(int start, int end){
    printf("PROGRAM:\n");
    printf("start pointer: %d; end pointer: %d\n", start, end);
    for(int i = start; i <= end; i++){
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

