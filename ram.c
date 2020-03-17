#include <stdlib.h>
#include <stdio.h>
#include <string.h>


//****PUBLIC VARIABLES****
char *ram[40];

//****PRIVATE VARIABLES****

//this is a flag that indicates that there isn't enough space to load the program into RAM
//this is used because addToRam must return void
//if loadErrorFlag == 1, then there wasn't enough space to load the program into RAM
int loadErrorFlag = 0;

//****PUBLIC METHODS****

//adds a new program to RAM
void addToRam(FILE *p, int* start, int* end){

    //find the first NULL space in RAM
    int k = 0;
    for(int i =0; i < 40; i++){
        if (ram[i] == NULL){
            *start = i;
            k = i;
            break;
        }
    }

    //get a line from the file
    char buffer[1000];

    //load the file into RAM line by line
    while(!feof(p)){
        memset(buffer, '\0', 999);
        fgets(buffer, 999, p);
        //remove blank lines
        if(strcmp(buffer, "") ==0) {
            continue;
        }
        ram[k] = strdup(buffer);
        k++;
        //if there isn't enough space in RAM, deal with the error
        if(k > 39 || ram[k] != NULL){
            *end = k - 1;
            //set the load error flag to 1, indicating that there isn't enough space in RAM
            loadErrorFlag = 1;
            return;
        }
    }

    *end = k - 1;

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

