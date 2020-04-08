#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//****PRIVATE VARIABLES****
char *ram[40];

//****PUBLIC METHODS****

//initializes RAM to NULL
void initRam(){
    for(int i = 0; i < 40; i++){
        ram[i] = NULL;
    }
}

//adds a line to index i of RAM
void addLineToRam(char* buffer, int i){
    if(ram[i] != NULL){
        free(ram[i]);
    }
    ram[i] = strdup(buffer);

}


//clears a program from RAM
void clearProgram(int start, int end){
    for(int i = start; i <= end; i++){
        if(ram[i] != NULL){
            free(ram[i]);
            ram[i] = NULL;
        }
    }
}


//returns a line from RAM
char * getLineFromRam(int line){
    return strdup(ram[line]);
}


