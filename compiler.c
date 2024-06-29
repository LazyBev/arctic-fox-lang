#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define STACK_DEPTH 1000
#define WORD_LENGTH 100
#define MAX_SIZE 100
#define MEM_CAP 640000

int label_counter = 0;
int last_block = 0;
int label_stack[MAX_SIZE];
int label_stack_index = 0;
int label_top = 0;

void compile(FILE *file, FILE *com) {
    char line[WORD_LENGTH * MAX_SIZE];
    char *word;

    if (!com) {
        fprintf(stderr, "Error opening output file\n");
        exit(1);
    }

    fprintf(com, "bits 64\n");
    fprintf(com, "section .bss\n");
    fprintf(com, "mem resb %d\n", MEM_CAP);
    fprintf(com, "digitSpace resb 64\n");
    fprintf(com, "digitSpacePos resq 1\n");
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

    while (fgets(line, sizeof(line), file)) {
        word = strtok(line, " \t\n");
        while (word != NULL) {
            fprintf(com, "addr_%d:\n", label_counter);
            if (strcmp(word, "+") == 0) {
                fprintf(com, "  ; -- plus --\n");
                fprintf(com, "  pop rax\n");
                fprintf(com, "  pop rbx\n");
                fprintf(com, "  add rax, rbx\n");
                fprintf(com, "  push rax\n");
            } else if (strcmp(word, "-") == 0) {
                fprintf(com, "  ; -- minus --\n");
                fprintf(com, "  pop rax\n");
                fprintf(com, "  pop rbx\n");
                fprintf(com, "  sub rbx, rax\n");
                fprintf(com, "  push rbx\n");
            } else if (strcmp(word, "*") == 0) {
                fprintf(com, "  ; -- multiply --\n");
                fprintf(com, "  pop rax\n");
                fprintf(com, "  pop rbx\n");
                fprintf(com, "  imul rax, rbx\n");
                fprintf(com, "  push rax\n");
            } else if (strcmp(word, "/") == 0) {
                fprintf(com, "  ; -- divide --\n");
                fprintf(com, "  pop rbx\n");
                fprintf(com, "  pop rax\n");
                fprintf(com, "  xor rdx, rdx\n");
                fprintf(com, "  div rbx\n");
                fprintf(com, "  push rax\n");
            } else if (strcmp(word, "%") == 0) {
                fprintf(com, "  ; -- modulus --\n");
                fprintf(com, "  pop rbx\n");
                fprintf(com, "  pop rax\n");
                fprintf(com, "  xor rdx, rdx\n");
                fprintf(com, "  div rbx\n");
                fprintf(com, "  push rdx\n");
            } else if (strcmp(word, "dump") == 0) {
                fprintf(com, "  ; -- dump --\n");
                fprintf(com, "  pop rax\n");
                fprintf(com, "  call printNum\n");
            } else if (strncmp(word, ":", 1) == 0) {
                int value = atoi(word + 1);
                fprintf(com, "  ; -- push %d --\n", value);
                fprintf(com, "  push %d\n", value);
            } else if (strcmp(word, ">") == 0) {
                fprintf(com, "  ; -- greater than --\n");
                fprintf(com, "  mov rcx, 0\n");
                fprintf(com, "  mov rdx, 1\n");
                fprintf(com, "  pop rax\n");
                fprintf(com, "  pop rbx\n");
                fprintf(com, "  cmp rax, rbx\n");
                fprintf(com, "  cmovg rcx, rdx\n");
                fprintf(com, "  push rcx\n");
            } else if (strcmp(word, ">=") == 0) {
                fprintf(com, "  ; -- greater than or equal to --\n");
                fprintf(com, "  mov rcx, 0\n");
                fprintf(com, "  mov rdx, 1\n");
                fprintf(com, "  pop rax\n");
                fprintf(com, "  pop rbx\n");
                fprintf(com, "  cmp rax, rbx\n");
                fprintf(com, "  cmovge rcx, rdx\n");
                fprintf(com, "  push rcx\n");
            } else if (strcmp(word, "!>") == 0) {
                fprintf(com, "  ; -- not greater than --\n");
                fprintf(com, "  mov rcx, 1\n");
                fprintf(com, "  mov rdx, 0\n");
                fprintf(com, "  pop rax\n");
                fprintf(com, "  pop rbx\n");
                fprintf(com, "  cmp rax, rbx\n");
                fprintf(com, "  cmovg rcx, rdx\n");
                fprintf(com, "  push rcx\n");
            } else if (strcmp(word, "<") == 0) {
                fprintf(com, "  ; -- less than --\n");
                fprintf(com, "  mov rcx, 0\n");
                fprintf(com, "  mov rdx, 1\n");
                fprintf(com, "  pop rax\n");
                fprintf(com, "  pop rbx\n");
                fprintf(com, "  cmp rax, rbx\n");
                fprintf(com, "  cmovl rcx, rdx\n");
                fprintf(com, "  push rcx\n");
            } else if (strcmp(word, "<=") == 0) {
                fprintf(com, "  ; -- less than or equal to --\n");
                fprintf(com, "  mov rcx, 0\n");
                fprintf(com, "  mov rdx, 1\n");
                fprintf(com, "  pop rax\n");
                fprintf(com, "  pop rbx\n");
                fprintf(com, "  cmp rax, rbx\n");
                fprintf(com, "  cmovle rcx, rdx\n");
                fprintf(com, "  push rcx\n");
            } else if (strcmp(word, "!<") == 0) {
                fprintf(com, "  ; -- not less than --\n");
                fprintf(com, "  mov rcx, 1\n");
                fprintf(com, "  mov rdx, 0\n");
                fprintf(com, "  pop rax\n");
                fprintf(com, "  pop rbx\n");
                fprintf(com, "  cmp rax, rbx\n");
                fprintf(com, "  cmovl rcx, rdx\n");
                fprintf(com, "  push rcx\n");
            } else if (strcmp(word, "=") == 0) {
                fprintf(com, "  ; -- equal --\n");
                fprintf(com, "  mov rcx, 0\n");
                fprintf(com, "  mov rdx, 1\n");
                fprintf(com, "  pop rax\n");
                fprintf(com, "  pop rbx\n");
                fprintf(com, "  cmp rax, rbx\n");
                fprintf(com, "  cmove rcx, rdx\n");
                fprintf(com, "  push rcx\n");
            } else if (strcmp(word, "!=") == 0) {
                fprintf(com, "  ; -- not equal to --\n");
                fprintf(com, "  mov rcx, 1\n");
                fprintf(com, "  mov rdx, 0\n");
                fprintf(com, "  pop rax\n");
                fprintf(com, "  pop rbx\n");
                fprintf(com, "  cmp rax, rbx\n");
                fprintf(com, "  cmove rcx, rdx\n");
                fprintf(com, "  push rcx\n");
            } else if (strcmp(word, "if") == 0) {
                last_block = ++label_counter;
                fprintf(com, "  ; -- if --\n");
                fprintf(com, "  pop rax\n");
                fprintf(com, "  test rax, rax\n");
                fprintf(com, "  jz addr_%d\n", last_block);
                label_stack[label_stack_index++] = last_block;
            } else if (strcmp(word, "else") == 0) {
                int else_label = ++label_counter;
                int if_label = label_stack[--label_stack_index];
                fprintf(com, "  ; -- else --\n");
                fprintf(com, "  jmp addr_%d\n", else_label);
                fprintf(com, "addr_%d:\n", if_label);3
                label_stack[label_stack_index++] = else_label;
            } else if (strcmp(word, "end") == 0) {
                int label = label_stack[--label_stack_index];
                fprintf(com, "  jmp addr_%d\n", label);
                fprintf(com, "addr_%d:\n", label);
            } else if (strcmp(word, "dup") == 0) {
                fprintf(com, "  ; -- duplicate top --\n");
                fprintf(com, "  pop rax\n");
                fprintf(com, "  push rax\n");
                fprintf(com, "  push rax\n");
            } else if (strcmp(word, "2dup") == 0) {
                fprintf(com, "  ; -- duplicate top --\n");
                fprintf(com, "  pop rax\n");
                fprintf(com, "  pop rbx\n");
                fprintf(com, "  push rax\n");
                fprintf(com, "  push rbx\n");
            } else if (strcmp(word, "mem") == 0) {
                fprintf(com, "  ; -- memory --\n");
                fprintf(com, "  push mem\n");
            } else if (strcmp(word, ",") == 0) {
                fprintf(com, "  ; -- load --\n");
                fprintf(com, "  pop rax\n");
                fprintf(com, "  xor rbx, rbx\n");
                fprintf(com, "  mov bl, [rax]\n");
                fprintf(com, "  push rbx\n");
            } else if (strcmp(word, ".") == 0) {
                fprintf(com, "  ; -- store --\n");
                fprintf(com, "  pop rbx\n");
                fprintf(com, "  pop rax\n");
                fprintf(com, "  mov [rax], bl\n");
            } else if (strcmp(word, "syscall1") == 0) {
                fprintf(com, "  ; -- syscall --\n");
                fprintf(com, "  pop rax\n");
                fprintf(com, "  pop rdi\n");
                fprintf(com, "  syscall\n");
            } else if (strcmp(word, "syscall3") == 0) {
                fprintf(com, "  ; -- syscall --\n");
                fprintf(com, "  pop rax\n");
                fprintf(com, "  pop rdi\n");
                fprintf(com, "  pop rsi\n");
                fprintf(com, "  pop rdx\n");
                fprintf(com, "  syscall\n");
            } else if (strcmp(word, "exit") == 0) {
                fprintf(com, "  mov rax, 60\n");
                fprintf(com, "  mov rdi, 0\n");
                fprintf(com, "  syscall\n");
            } else if (strcmp(word, "//") == 0) {
                break;
            } else {
                printf("Syntax Error: Invalid token: %s\n", word);
                exit(1);
            }
            word = strtok(NULL, " \t\n");
            label_counter++;
        }
    }

}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <subcommand> <in> <out>\n", argv[0]);
        fprintf(stderr, "Subcommand: -c: Compile program\n");
        return 1;
    }

    FILE *file = fopen(argv[2], "r");
    char outfile[MAX_SIZE];
    snprintf(outfile, sizeof(outfile), "%s.s", argv[3]);
    FILE *out = fopen(outfile, "w+");

    if (file == NULL) {
        printf("Error opening file: %s\n", argv[2]);
        return 1;
    }

    if (strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "-r") == 0) {
        compile(file, out);
        fclose(out);

        char cmd[MAX_SIZE];
        snprintf(cmd, sizeof(cmd), "nasm -felf64 -o %s.o %s.s", argv[3], argv[3]);
        printf("%s\n", cmd);
        system(cmd);

        snprintf(cmd, sizeof(cmd), "ld -o %s %s.o", argv[3], argv[3]);
        printf("%s\n", cmd);
        system(cmd);

        if (strcmp(argv[1], "-r") == 0) {
            snprintf(cmd, sizeof(cmd), "./%s", argv[3]);
            printf("%s\n", cmd);
            system(cmd);
        }
    } else {
        fprintf(stderr, "Invalid subcommand: %s\n", argv[1]);
    }

    fclose(file);
    return 0;
}
