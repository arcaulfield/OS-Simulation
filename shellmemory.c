#include <string.h>
#include <stdlib.h>
#include "shell.h"

//****PRIVATE VARIABLES****

struct MEM {
    char* var;
    char* val;
};

struct MEM** memory;

//****PUBLIC METHODS****

void initMemory(){
    memory = (struct MEM **) malloc(1000 * sizeof(struct MEM*));
    for(int i = 0; i < 1000; i ++){
        memory[i] = NULL;
    }
}
//gets the value of a variable saved in memory
char* getVal(char* var){
    int i = 0;
    for (i = 0; i < 1000; i++){
        if(memory[i] != NULL){
            if (strcmp(memory[i]->var, var) == 0){
                return memory[i]->val;
            }
        }
    }
    return "Variable does not exist";
}

//sets the value stored at a variable
void setVal(char* var, char* val){
    for(int i = 0; i < 1000; i++){
        if (memory[i] == NULL){
            struct MEM *new = (struct MEM *) malloc(sizeof(struct MEM));
            new->val = (char *) malloc(word_length * sizeof(char));
            new->var = (char *) malloc(word_length * sizeof(char));
            memset(new->val, '\0', word_length);
            memset(new->var, '\0', word_length);
            strcpy(new->val, val);
            strcpy(new->var, var);
            memory[i] = new;
            return;
        }
        else {
            if (strcmp(memory[i]->var, var) == 0){
                strcpy(memory[i]->val, val);
                return;
            }
        }
    }
}

//Clears memory. This is called when the program terminates.
void clearMemory(){
    for(int i = 0; i < 1000; i++){
        if(memory[i] != NULL){
            free(memory[i]->val);
            free(memory[i]->var);
            free(memory[i]);
        }
    }
    free(memory);
}