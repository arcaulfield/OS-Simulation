#ifndef SHELLMEMORY_H
#define SHELLMEMORY_H

extern int shellFullFlag;

void setVal(char* var, char* val);
char* getVal(char* var);
void clearMemory();
void initMemory();

#endif
