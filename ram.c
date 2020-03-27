#include <stdlib.h>
#include <stdio.h>
#include <string.h>



//****PRIVATE VARIABLES****
char *ram[40];

//this is a flag that indicates that there isn't enough space to load the program into RAM
//this is used because addToRam must return void
//if loadErrorFlag == 1, then there wasn't enough space to load the program into RAM
int loadErrorFlag = 0;

//****PUBLIC METHODS****

//initializes RAM to NULL
void initRam(){
    for(int i = 0; i < 40; i++){
        ram[i] = NULL;
    }
}

//adds a line to RAM
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

//returns the value of the load error flag
int checkErrorFlag(){
    return loadErrorFlag;
}

//resets the load error flag value to 0 (indicating that there is no error)
void resetFlag(){
    loadErrorFlag = 0;
}

//returns a line from RAM
char * getLineFromRam(int line){
    return strdup(ram[line]);
}

//Prints the contents of RAM
//This is only used for debugging purposes
void printRam(int start, int end){
    printf("PROGRAM:\n");
    printf("start pointer: %d; end pointer: %d\n", start, end);
    for(int i = start; i <= end; i++){
        printf("%s", ram[i]);
    }
}

