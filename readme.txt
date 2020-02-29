For this assignment, I used mimi.cs.mcgill.ca

I assume that when there is a load error of any kind (i.e. a script name duplicate, not enough space in ram, script not found), no programs are executed, as specified in the assignment description.

Furthermore, I call parse(IR) rather than interpreter(IR), in the run function of the CPU, which differs from the assignment 2 specification. I do this because my parser parses the input, then calls the interpreter, as was specified for assignment 1.

As done with the run function in assignment 1, if a syntax error occurs while executing a program (for example if there is a unknown command or file that can’t be found), the program is terminated. As with assignment 1, an unknown variable isn’t considered a syntax error.

The bash script “compileMyKernel.sh” compiles the program into an executable. 