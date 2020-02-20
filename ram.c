#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *ram[1000];

//this is a flag that indicates that there isn't enough space to load the program into RAM
//this is used because addToRam must return void
int loadErrorFlag = 0;

int checkErrorFlag(){
    return loadErrorFlag;
}

//initializes RAM
void initRam(){
    for(int i = 0; i < 1000; i++){
        ram[i] = NULL;
    }
}

void addToRam(FILE *p, int* start, int* end){
    int k = 0;
    for(int i =0; i < 1000; i++){
        if (ram[i] == NULL){
            *start = i;
            k = i;
            break;
        }
    }

    char buffer[1000];
    fgets(buffer, 999, p);

    while(!feof(p)){
        ram[k] = (char *) malloc(1000 * sizeof(char));
        ram[k] = strdup(buffer);
        k++;
        if(k > 999 || ram[k] != NULL){
            *end = k - 1;
            fclose(p);
            loadErrorFlag = 1;
            return;
        }
        fgets(buffer, 999, p);
    }

    *end = k - 1;
    fclose(p);
}
//print function added for debugging purposes
void printRam(int start, int end){
    printf("PROGRAM:\n");
    printf("start pointer: %d; end pointer: %d\n", start, end);
    for(int i = start; i <= end; i++){
        printf("%s", ram[i]);
    }
}


void clearProgram(int start, int end){
    for(int i = start; i <= end; i++){
        free(ram[i]);
        ram[i] = NULL;
    }
}

void resetFlag(){
    loadErrorFlag = 0;
    //clear all of the RAM
    clearProgram(0, 999);
}

//clears all the memory from ram
//called when program finishes running
void clearAllRam(){
    for(int i = 0; i < 1000; i ++){
        free(ram[i]);
    }
}

//returns a line from RAM
char * getLineFromRam(int line){
    return strdup(ram[line]);
}

