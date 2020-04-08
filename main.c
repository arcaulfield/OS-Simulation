# include "kernel.h"

int main(){
    int error = 0;
    boot();
    error = kernel();
    return error;
}
