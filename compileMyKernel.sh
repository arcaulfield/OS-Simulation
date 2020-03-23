#!/bin/bash

gcc -c ram.c kernel.c cpu.c shell.c interpreter.c pcb.c shellmemory.c memorymanager.c
gcc -o mykernel ram.o kernel.o cpu.o shell.o interpreter.o pcb.o shellmemory.o memorymanager.o
