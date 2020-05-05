# OS Simulation

This Unix-based OS simulation consists of a shell, a simple boot sequence, uses virtual memory and process scheduling. 

<p align="center">
<img src="https://github.com/arcaulfield/OS-Simulation/blob/master/kernel_running.png" width="400"/>
</p>

## Shell Implementation
The simulation includes a simple shell, which allows for user interaction. The possible commands include 
- ````help````: prints all possible commands for this OS
- ````set VAR_NAME VALUE````: sets the value  
- ````print VAR_NAME````: prints the value stored in VAR_NAME to the screen
- ````run SCRIPT.TXT````: runs the script SCRIPT.TXT
- ````exec SCRIPT1.TXT SCRIPT2.TXT SCRIPT3.TXT````: executes up to 3 scripts simuletaneously 
- ````quit````: terminates the shell

The simulation assumes that all scripts consist of the above commands, with at most one command per line. The simulation handles unknown commands, by printing an error to the user. The shell also has a simple memory, which stores the variable values. 

## Process Execution

The simulation implements a time-sharing OS. Therefore, it includes a CPU that can run at most one process at a time. The simulation implements a FIFO ready queue. Up to 3 processes can execute "simultaneously". Each process is tracked using a Process Control Block (PCB). When executing, the Scheduler will select the process at the beginning of the ready queue. This process will run on the CPU for one quanta, which is considered to be 2 lines of code. Afterwards, the process either terminates or is placed at the back of the ready queue. 

## Memory Management

For this simulation, memory consists of RAM, which is 10 frames, as well as a Backing Store. A directory called BackingStore is used to simulate a Backing Store and RAM is simulated using an array. The simulation includes a Memory Manager, which oversees a basic paging system. Whenever a program is executed, the Memory Manager loads the new program into the run time environment. It copies the entire program into the Backing Store, then loads at most 2 pages of the program into the RAM. Each process has a page table, which is stored in the process's PCB. The Memory Manager also handles page faults and page victim selection. 


## Boot Sequence

The Boot Sequence consists of 
1. Preparing the RAM
2. Preparing the BackingStore

and is the first thing to execute when the simulation is launched. 

## Give it a try! 

In order to run the OS, after cloning the repo,
1. Run ````./compileMyKernel.sh ````
2. Run ````./mykernel ````
3. Enjoy! 
