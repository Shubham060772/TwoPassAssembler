// emulator.c

#include <stdio.h>
#include "simple.h"

// Memory and registers
int memory[1024] = {0};
int pc = 0, sp = 0, a = 0, b = 0;

void load_program(const char *filename) {
    FILE *file = fopen(filename, "rb");
    int address = 0;
    
    while (fread(&memory[address], sizeof(int), 1, file) == 1) {
        address++;
    }
    fclose(file);
}

void execute_program(void) {
    while (1) {
        int instruction = memory[pc++];
        int opcode = instruction & 0xFF;
        int operand = instruction >> 8;

        switch (opcode) {
            case 0: b = a; a = operand; break;  // ldc
            case 1: a += operand; break;        // adc
            case 18: return;                    // HALT
            // Add more cases for other opcodes
        }
    }
}
