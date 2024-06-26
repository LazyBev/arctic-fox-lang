#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define STACK_DEPTH 1000
#define WORD_LENGTH 100
#define MAX_SIZE 100

int label_counter = 0;
int last_block = 1;
void compile_program(char *word, FILE *com) {
    if (strncmp(word, "@", 1) == 0) {
        if (strcmp(word + 1, "out") == 0) {
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
        } else {
            printf("Syntax Error: Unknown @ directive: %s\n", word);
            exit(1);
        }
    } else if (strcmp(word, "+") == 0) {
        fprintf(com, "  ; -- plus --\n");
        fprintf(com, "  pop rax\n");
        fprintf(com, "  pop rbx\n");
        fprintf(com, "  add rax, rbx\n");
        fprintf(com, "  push rax\n");
    } else if (strcmp(word, "-") == 0) {
        fprintf(com, "  ; -- minus --\n");
        fprintf(com, "  pop rax\n");
        fprintf(com, "  pop rbx\n");
        fprintf(com, "  sub rax, rbx\n");
        fprintf(com, "  push rax\n");
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
    } else if (strcmp(word, ".") == 0) {
        fprintf(com, "  ; -- dump --\n");
        fprintf(com, "  pop rax\n");
        fprintf(com, "  call printNum\n");
    } else if (strcmp(word, "exit") == 0) {
        fprintf(com, "  mov rax, 60\n");
        fprintf(com, "  mov rdi, 0\n");
        fprintf(com, "  syscall\n");
    } else if (strncmp(word, ":", 1) == 0) {
        if (strcmp(word + 1, "dup") == 0) {
            fprintf(com, "  ; -- duplicate top --\n");
            fprintf(com, "  pop rax\n");
            fprintf(com, "  push rax\n");
            fprintf(com, "  push rax\n");
        } else {
            int value = atoi(word + 1);
            fprintf(com, "  ; -- push %d --\n", value);
            fprintf(com, "  push %d\n", value);
        }
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
    } else if (strcmp(word, "?") == 0) {
        fprintf(com, "  ; -- if --\n");
        fprintf(com, "  pop rax\n");
        fprintf(com, "  test rax, rax\n");
        fprintf(com, "  jz else_block_%d\n", last_block);
    } else if (strcmp(word, "~") == 0) {
        fprintf(com, "  ; -- else --\n");
        fprintf(com, "  jmp end_if_%d\n", last_block);
        fprintf(com, "else_block_%d:\n", last_block);
    } else if (strcmp(word, ";") == 0) {
        fprintf(com, "  ; -- end --\n");
        fprintf(com, "end_if_%d:\n", last_block);
        last_block++;
    } else if (strcmp(word, "dup") == 0) {
        fprintf(com, "  ; -- duplicate top --\n");
        fprintf(com, "  pop rax\n");
        fprintf(com, "  push rax\n");
        fprintf(com, "  push rax\n");
    } else {
        printf("Syntax Error: Invalid token: %s\n", word);
        exit(1);
    }
}

void compile_file(FILE *file, FILE *com) {
    char line[WORD_LENGTH * MAX_SIZE];
    char *word;
    bool exit_found = false;

    if (!com) {
        fprintf(stderr, "Error opening output file");
        exit(1);
    }

    fprintf(com, "section .bss\n");
    fprintf(com, "digitSpace resb 64\n");
    fprintf(com, "digitSpacePos resq 1\n");
    fprintf(com, "section .text\n");
    fprintf(com, "global _start\n");
    fprintf(com, "_start:\n");
    
    while (fgets(line, sizeof(line), file)) {
        word = strtok(line, " \t\n");
        while (word != NULL) {
            compile_program(word, com);
            if (strcmp(word, "exit") == 0) {
                exit_found = true;
                break;
            }
            word = strtok(NULL, " \t\n");
            label_counter++;
        }
        if (exit_found) {
            break;
        }
    }

    if (!exit_found) {
        printf("Error: 'exit' not found at the end of file\n");
        fclose(com);
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <subcommand> <in> <out>\n", argv[0]);
        fprintf(stderr, "Subcommand: -c: Compile program\n");
        // fprintf(stderr, "Subcommand: -r: Compile && Run program\n");
        fprintf(stderr, "Subcommand: -i: Interpret program\n");
        return 1;
    }

    FILE *file = fopen(argv[2], "r");
    FILE *out = fopen(argv[3], "w+");

    if (file == NULL) {
        printf("Error opening file: %s\n", argv[2]);
        return 1;
    }

    if (strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "-r") == 0) {
        compile_file(file, out);
        fclose(out);

        /*char cmd[MAX_SIZE];
        snprintf(cmd, sizeof(cmd), "nasm -felf64 %s -o %s.o", argv[3], argv[3]);
        printf("%s\n", cmd);
        system(cmd);

        snprintf(cmd, sizeof(cmd), "ld -o %s %s.o", argv[3], argv[3]);
        printf("%s\n", cmd);
        system(cmd);

        if (strcmp(argv[1], "-r") == 0) {
            snprintf(cmd, sizeof(cmd), "./%s", argv[3]);
            printf("%s\n", cmd);
            system(cmd);
        }*/
    } else {
        fprintf(stderr, "Invalid subcommand: %s\n", argv[1]);
    }

    fclose(file);
    return 0;
}
