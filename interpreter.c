#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "shellmemory.h"
#include "shell.h"
#include "ram.h"

//prints a help menu
int help(){
    printf("POSSIBLE COMMANDS FOR THE ALISON SHELL:\nhelp - displays all the commands\n");
    printf("quit - exits/terminates the shell\nset VAR STRING - assigns a value to shell memory\n");
    printf("print VAR - displays the STRING assigned to VAR\n");
    printf("run SCRIPT.TXT - executes the file SCRIPT.TXT\n");
    return 0;
}

//Terminates program.
//Note that, by assumption, this will still terminate the program if called by a script.
int quit(){
    printf("Bye!\n");
    clearMemory();
    exit(0);
    return 0;
}

//sets the value stored in variables in memory
int set(char** words){
    char *var = words[1];
    char *val = words[2];;
    setVal(var, val);
    return 0;
}

//prints the values stored in variables in memory
int print(char ** words){
    char *var = words[1];
    char *val = getVal(var);
    printf("%s\n", val);
    return 0;
}

//runs scripts
int run(char **words){

    char *filename = words[1];

    int errorCode = 0;
    char line[1000];

    FILE *p = fopen(filename, "rt");
    if(p == NULL){
        errorCode = 2;
        return errorCode;
    }

    fgets(line, 999, p);
    while(!feof(p)){
        errorCode = parse(line);
        if(errorCode != 0){
            fclose(p);
            return errorCode;
        }
        fgets(line, 999, p);
    }
    fclose(p);
    return errorCode;

}

//executes programs simultaneously
int exec(char** words){
    char* filename = words[1];
    char* filename2 = words[2];
    loadProgram(filename);
    loadProgram(filename2);
    printRam();
    return 0;
}

int interpreter(char ** words){
    int errorCode = 0;
    char *argument = words[0];

    if (strcmp(argument, "help") == 0) errorCode = help();
    else if (strcmp(argument, "quit") == 0) errorCode = quit();
    else if (strcmp(argument, "set") == 0) errorCode = set(words);
    else if (strcmp(argument, "print") == 0) errorCode = print(words);
    else if (strcmp(argument, "run") == 0) errorCode = run(words);
    else if (strcmp(argument, "exec") == 0) errorCode = exec(words);
    else errorCode = 1;

    return errorCode;
}
