// simple.h

#ifndef SIMPLE_H
#define SIMPLE_H

#define MAX_LABELS 100

// Instruction structure
typedef struct {
    char *mnemonic;
    int opcode;
    int has_operand; // 1 if the instruction requires an operand, 0 otherwise
} Instruction;

// Symbol table entry for labels
typedef struct {
    char label[20];
    int address;
} Symbol;

// Functions for the assembler
void first_pass(FILE *source);
void second_pass(FILE *source, FILE *obj_file, FILE *lst_file);
int add_label(const char *label, int address);
int find_label(const char *label);

// Emulator functions
void load_program(const char *filename);
void execute_program(void);

// Memory and registers for the emulator
extern int memory[1024];
extern int pc, sp, a, b;

#endif // SIMPLE_H
