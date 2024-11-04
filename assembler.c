// assembler.c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "simple.h"

// Initialize memory for labels
Symbol symbol_table[MAX_LABELS];
int symbol_count = 0;

Instruction instruction_table[] = {
    {"ldc", 0, 1}, {"adc", 1, 1}, {"ldl", 2, 1}, {"stl", 3, 1},
    {"ldnl", 4, 1}, {"stnl", 5, 1}, {"add", 6, 0}, {"sub", 7, 0},
    {"shl", 8, 0}, {"shr", 9, 0}, {"adj", 10, 1}, {"a2sp", 11, 0},
    {"sp2a", 12, 0}, {"call", 13, 1}, {"return", 14, 0},
    {"brz", 15, 1}, {"brlz", 16, 1}, {"br", 17, 1}, {"HALT", 18, 0},
    {"data", -1, 1}, {"SET", -2, 1}
};

int find_instruction(const char *mnemonic) {
    for (int i = 0; i < sizeof(instruction_table) / sizeof(Instruction); i++) {
        if (strcmp(instruction_table[i].mnemonic, mnemonic) == 0) {
            return i;
        }
    }
    return -1;  // Not found
}

int add_label(const char *label, int address) {
    if (symbol_count < MAX_LABELS) {
        strcpy(symbol_table[symbol_count].label, label);
        symbol_table[symbol_count].address = address;
        symbol_count++;
        return 0;
    }
    return -1;  // Table overflow
}

int find_label(const char *label) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].label, label) == 0) {
            return symbol_table[i].address;
        }
    }
    return -1;
}

void first_pass(FILE *source) {
    char line[256];
    int address = 0;

    while (fgets(line, sizeof(line), source)) {
        char label[20], mnemonic[10], operand[20];

        if (sscanf(line, "%[^:]:%s%s", label, mnemonic, operand) == 3) {
            add_label(label, address);
        } else if (sscanf(line, "%[^:]:%s", label, mnemonic) == 2) {
            add_label(label, address);
        } else if (sscanf(line, "%s%s", mnemonic, operand) == 2) {
            if (find_instruction(mnemonic) >= 0) {
                address++;
            }
        }
    }
}

void second_pass(FILE *source, FILE *obj_file, FILE *lst_file) {
    char line[256];
    int address = 0;

    while (fgets(line, sizeof(line), source)) {
        char label[20], mnemonic[10], operand[20];
        int opcode, operand_value = 0, has_operand = 0;

        if (sscanf(line, "%[^:]:%s%s", label, mnemonic, operand) == 3 || 
            sscanf(line, "%s%s", mnemonic, operand) == 2) {
            int inst_index = find_instruction(mnemonic);
            if (inst_index < 0) continue;
            opcode = instruction_table[inst_index].opcode;
            has_operand = instruction_table[inst_index].has_operand;

            if (has_operand) {
                if (isalpha(operand[0])) {
                    operand_value = find_label(operand);
                } else {
                    operand_value = atoi(operand);
                }
            }

            int machine_code = (operand_value << 8) | opcode;
            fwrite(&machine_code, sizeof(int), 1, obj_file);

            fprintf(lst_file, "%08X %08X %s %s\n", address, machine_code, mnemonic, operand);
            address++;
        }
    }
}
