// main.c

#include <stdio.h>
#include <stdlib.h>
#include "simple.h"

void assemble(const char *filename) {
    FILE *source = fopen(filename, "r");
    if (!source) {
        perror("Error opening source file");
        return;
    }
    char obj_filename[256], lst_filename[256];
    snprintf(obj_filename, sizeof(obj_filename), "%s.obj", filename);
    snprintf(lst_filename, sizeof(lst_filename), "%s.lst", filename);

    FILE *obj_file = fopen(obj_filename, "wb");
    FILE *lst_file = fopen(lst_filename, "w");
    
    first_pass(source);
    rewind(source);
    second_pass(source, obj_file, lst_file);
    
    fclose(source);
    fclose(obj_file);
    fclose(lst_file);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <assembly_file>\n", argv[0]);
        return 1;
    }

    assemble(argv[1]);
    load_program("output.obj");
    execute_program();

    return 0;
}
