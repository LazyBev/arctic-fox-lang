#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 256

enum {
    OP_PUSH,
    OP_PLUS,
    OP_MINUS,
    OP_DIVIDE,
    OP_MODULUS,
    OP_DUMP,
    OP_EQUAL,
    OP_IF,
    OP_ELSE,
    OP_END,
};

typedef struct {
    int op;
    int value;
} Instruction;

Instruction push(int x) {
    Instruction inst = {OP_PUSH, x};
    return inst;
}

Instruction plus() {
    Instruction inst = {OP_PLUS, 0};
    return inst;
}

Instruction divide() {
    Instruction inst = {OP_DIVIDE, 0};
    return inst;
}

Instruction modulus() {
    Instruction inst = {OP_MODULUS, 0};
    return inst;
}

Instruction equal() {
    Instruction inst = {OP_EQUAL, 0};
    return inst;
}

Instruction iff() {
    Instruction inst = {OP_IF, 0};
    return inst;
}

Instruction elze() {
    Instruction inst = {OP_ELSE, 0};
    return inst;
}

Instruction end() {
    Instruction inst = {OP_END, 0};
    return inst;
}

Instruction minus() {
    Instruction inst = {OP_MINUS, 0};
    return inst;
}

Instruction dump() {
    Instruction inst = {OP_DUMP, 0};
    return inst;
}

void compile_program(Instruction program[], int length, const char *name) {
    char out[MAX_SIZE];
    snprintf(out, sizeof(out), "build/%s.s", name);
    FILE *com = fopen(out, "w+");
    if (!com) {
        fprintf(stderr, "Error: Unable to open file %s for writing\n", out);
        return;
    }

    fprintf(com, "bits 64\n");
    fprintf(com, "section .bss\n");
    fprintf(com, "  buffer resb 20\n");
    fprintf(com, "  digitSpace resb 100\n");
    fprintf(com, "  digitSpacePos resb 8\n");
    
    fprintf(com, "section .data\n");
    fprintf(com, "  newline db 0xA\n");

    fprintf(com, "section .text\n");
    fprintf(com, "printNum:\n");
    fprintf(com, "  mov rcx, digitSpace\n");
    fprintf(com, "  mov rbx, 10\n");
    fprintf(com, "  mov [rcx], rbx\n");
    fprintf(com, "  inc rcx\n");
    fprintf(com, "  mov [digitSpacePos], rcx\n");

    fprintf(com, "printNumLoop:\n");
    fprintf(com, "  mov rdx, 0\n");
    fprintf(com, "  mov rbx, 10\n");
    fprintf(com, "  div rbx\n");
    fprintf(com, "  push rax\n");
    fprintf(com, "  add rdx, 48\n");
    fprintf(com, "  mov rcx, [digitSpacePos]\n");
    fprintf(com, "  mov [rcx], dl\n");
    fprintf(com, "  inc rcx\n");
    fprintf(com, "  mov [digitSpacePos], rcx\n");
    fprintf(com, "  pop rax\n");
    fprintf(com, "  cmp rax, 0\n");
    fprintf(com, "  jne printNumLoop\n");

    fprintf(com, "printNumLoop2:\n");
    fprintf(com, "  mov rcx, [digitSpacePos]\n");
    fprintf(com, "  mov rax, 1\n");
    fprintf(com, "  mov rdi, 1\n");
    fprintf(com, "  mov rsi, rcx\n");
    fprintf(com, "  mov rdx, 1\n");
    fprintf(com, "  syscall\n");
    fprintf(com, "  mov rcx, [digitSpacePos]\n");
    fprintf(com, "  dec rcx\n");
    fprintf(com, "  mov [digitSpacePos], rcx\n");
    fprintf(com, "  cmp rcx, digitSpace\n");
    fprintf(com, "  jge printNumLoop2\n");
    fprintf(com, "  ret\n");
    fprintf(com, "global _start\n");
    fprintf(com, "_start:\n");

    int if_stack[MAX_SIZE];
    int sp = 0; 

    for (int ip = 0; ip < length; ip++) {
        Instruction inst = program[ip];
        switch (inst.op) {
            case OP_PUSH:
                fprintf(com, "  ;; -- push %d --\n", inst.value);
                fprintf(com, "  push %d\n", inst.value);
                break;
            case OP_PLUS:
                fprintf(com, "  ;; -- plus --\n");
                fprintf(com, "  pop rax\n");
                fprintf(com, "  pop rbx\n");
                fprintf(com, "  add rax, rbx\n");
                fprintf(com, "  push rax\n");
                break;
            case OP_DIVIDE:
                fprintf(com, "  ;; -- divide --\n");
                fprintf(com, "  pop rbx\n");
                fprintf(com, "  pop rax\n");
                fprintf(com, "  xor rdx, rdx\n");
                fprintf(com, "  div rbx\n");
                fprintf(com, "  push rax\n");
                break;
            case OP_MODULUS:
                fprintf(com, "  ;; -- modulus --\n");
                fprintf(com, "  pop rbx\n");
                fprintf(com, "  pop rax\n");
                fprintf(com, "  xor rdx, rdx\n");
                fprintf(com, "  div rbx\n");
                fprintf(com, "  push rdx\n");
                break;
            case OP_MINUS:
                fprintf(com, "  ;; -- minus --\n");
                fprintf(com, "  pop rax\n");
                fprintf(com, "  pop rbx\n");
                fprintf(com, "  sub rbx, rax\n");
                fprintf(com, "  push rbx\n");
                break;
            case OP_EQUAL:
                fprintf(com, "  ;; -- equal --\n");
                fprintf(com, "  mov rcx, 0\n");
                fprintf(com, "  mov rdx, 1\n");
                fprintf(com, "  pop rax\n");
                fprintf(com, "  pop rbx\n");
                fprintf(com, "  cmp rax, rbx\n");
                fprintf(com, "  cmove rcx, rdx\n");
                fprintf(com, "  push rcx\n");
                break;
            case OP_IF:
                fprintf(com, "  ;; -- if --\n");
                fprintf(com, "  pop rax\n");
                fprintf(com, "  cmp rax, 0\n");
                fprintf(com, "  jne _end_%d\n", ip++);
                if_stack[sp++] = ip;
                break;
            case OP_ELSE:
                fprintf(com, "  ;; -- else --\n");
                fprintf(com, "  je _else_%d\n", ip++);
                fprintf(com, "_else_%d:\n", ip++);

                break;
            case OP_END:
                fprintf(com, "  ;; -- end --\n");
                fprintf(com, "_end_%d:\n", ip++);
                break;
            default:
                fprintf(stderr, "Error: unknown operation %d\n", inst.op);
                fclose(com);
                return;
        }
    }
    fprintf(com, "  mov rax, 60\n");
    fprintf(com, "  mov rdi, 0\n");
    fprintf(com, "  syscall\n");

    fclose(com);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <subcommand> <file>.afl\n", argv[0]);
        fprintf(stderr, "Subcommand: -c: Compile program to assembly\n");
        fprintf(stderr, "Subcommand: -r: Compile && Run program\n");
        return 1;
    }

    const char *subcommand = argv[1];
    const char *input_file = argv[2];

    const char *file_extension = ".afl";
    size_t input_file_len = strlen(input_file);
    size_t extension_len = strlen(file_extension);

    if (input_file_len < extension_len || strcmp(input_file + input_file_len - extension_len, file_extension) != 0) {
        fprintf(stderr, "Error: Input file must have extension .afl\n");
        return 1;
    }

    FILE *file_handle = fopen(input_file, "r");
    if (!file_handle) {
        fprintf(stderr, "Error opening file: %s\n", input_file);
        return 1;
    }

    char name[MAX_SIZE];
    strncpy(name, input_file, strlen(input_file) - 4);
    name[strlen(input_file) - 4] = '\0';

    Instruction program[MAX_SIZE];
    int program_length = 0;
    char line[MAX_SIZE];

    while (fgets(line, sizeof(line), file_handle)) {
        
        if (strncmp(line, ";;", 2) == 0) {
            continue;
        }

        char *token = strtok(line, " \t\n");
        while (token != NULL) {
            if (strcmp(token, "+") == 0) {
                program[program_length++] = plus();
            } else if (strcmp(token, "-") == 0) {
                program[program_length++] = minus();
            } else if (strcmp(token, "/") == 0) {
                program[program_length++] = divide();
            } else if (strcmp(token, "\%") == 0) {
                program[program_length++] = modulus();
            } else if (strcmp(token, "=") == 0) {
                program[program_length++] = equal();
            } else if (strcmp(token, "?") == 0) {
                program[program_length++] = iff();
            } else if (strcmp(token, "~") == 0) {
                program[program_length++] = elze();
            } else if (strcmp(token, ";") == 0) {
                program[program_length++] = end();
            } else if (strcmp(token, ".") == 0) {
                program[program_length++] = dump();
            } else {
                int value = atoi(token);
                program[program_length++] = push(value);
            }
            token = strtok(NULL, " \t\n");
        }
    }

    fclose(file_handle);

    if (strcmp(subcommand, "-c") == 0) {
        compile_program(program, program_length, name);
        char cmd[MAX_SIZE];
        snprintf(cmd, sizeof(cmd), "nasm -felf64 build/%s.s -o build/%s.o", name, name);
        printf("%s\n", cmd);
        system(cmd);

        snprintf(cmd, sizeof(cmd), "ld -o build/%s build/%s.o", name, name);
        printf("%s\n", cmd);
        system(cmd);
    } else if (strcmp(subcommand, "-r") == 0) {
        compile_program(program, program_length, name);
        char cmd[MAX_SIZE];
        snprintf(cmd, sizeof(cmd), "nasm -felf64 build/%s.s -o build/%s.o", name, name);
        printf("%s\n", cmd);
        system(cmd);

        snprintf(cmd, sizeof(cmd), "ld -o build/%s build/%s.o", name, name);
        printf("%s\n", cmd);
        system(cmd);

        snprintf(cmd, sizeof(cmd), "./build/%s", name);
        printf("%s\n", cmd);
        system(cmd);
    } else {
        fprintf(stderr, "Invalid subcommand: %s\n", subcommand);
        return 1;
    }

    return 0;
}
