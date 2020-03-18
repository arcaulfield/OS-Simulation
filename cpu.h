#ifndef ASSIGNMENT2_CPU_H
#define ASSIGNMENT2_CPU_H

extern int pageFaultFlag;

int cpuAvailable();
void initCPU();
int run(int quanta);
void updateIP(int ip, int offset);
void freeMyCPU();

#endif //ASSIGNMENT2_CPU_H
