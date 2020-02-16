#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "shellmemory.h"
#include "shell.h"
#include "ram.h"
#include "kernel.h"

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
    int i = 0;
    int errorCode = 0;
    char* filename = words[1];
    errorCode = myinit(filename);

    //parse words[2] to get 2 file names

    int count  = 0;
    while(words[2][i] != '\0'){
        while(words[2][i] == ' '){
            i ++;
        }
        if(words[2][i] == '\0'){
            break;
        }
        count ++;
        if(count > 2){
            printf("WARNING: only 3 programs can be executed at once. Only the first three inputs will be executed.\n");
            break;
        }
        memset(filename, '\0', word_length);
        int j = 0;
        while(words[2][i] != ' ' && words[2][i] != '\0'){
            filename[j] = words[2][i];
            i++;
            j++;
        }
        filename[j] = '\0';
        myinit(filename);
    }


    //char* filename2 = words[2];

   // myinit(filename);
    //printRam();
   // myinit(filename2);
    //printRam();
    return errorCode;
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
