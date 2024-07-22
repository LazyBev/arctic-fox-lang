#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>

#define MEM_CAP 4096
#define MAX_SIZE 1000
#define LINE_SIZE 256
#define ASM_FILE "out.s"

int i;

typedef struct {
    char *value;
    int type; // 0 = number, 1 = operator, 2 = keyword
} Token;

Token *tokenize(char *line) {
    Token *tokens = malloc(sizeof(Token) * MAX_SIZE);
    int token_count = 0;

    char *token = strtok(line, " \t\n");
    while (token != NULL) {
        Token t;
        t.value = strdup(token);

        if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1]))) {
            t.type = 0;
        } else if (strcmp(token, "+") == 0 || strcmp(token, "-") == 0 || 
                   strcmp(token, "*") == 0 || strcmp(token, "/") == 0 || 
                   strcmp(token, "%") == 0) {
            t.type = 1;
        } else if (strcmp(token, "//") == 0) {
            t.type = 3; // comment
            break;
        } else {
            t.type = 2;
        }

        tokens[token_count++] = t;
        token = strtok(NULL, " \t\n");
    }

    tokens[token_count].value = NULL;
    return tokens;
}

int int_len(int num) {
    int len = 0;

    if (num == 0) return 1;
    if (num < 0) num = -num;

    while (num != 0) {
        num /= 10;
        ++len;
    }
    
    return len;
}

void generate_asm(Token *token, FILE *out) {
    switch (token->type) {
        case 0: // number
            fprintf(out, "    ;-- push --\n");
            fprintf(out, "    push %d\n", atoi(token->value));
            break;

        case 1: // operator
            if (strcmp(token->value, "+") == 0) {
                fprintf(out, "    ;-- add --\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    pop rbx\n");
                fprintf(out, "    add rax, rbx\n");
                fprintf(out, "    push rax\n");
            } else if (strcmp(token->value, "-") == 0) {
                fprintf(out, "    ;-- sub --\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    pop rbx\n");
                fprintf(out, "    sub rbx, rax\n");
                fprintf(out, "    push rbx\n");
            } else if (strcmp(token->value, "*") == 0) {
                fprintf(out, "    ;-- mul --\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    pop rbx\n");
                fprintf(out, "    imul rax, rbx\n");
                fprintf(out, "    push rax\n");
            } else if (strcmp(token->value, "/") == 0) {
                fprintf(out, "    ;-- div --\n");
                fprintf(out, "    xor rdx, rdx\n");
                fprintf(out, "    pop rbx\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    div rbx\n");
                fprintf(out, "    push rax\n");
            } else if (strcmp(token->value, "%") == 0) {
                fprintf(out, "    ;-- mod --\n");
                fprintf(out, "    xor rdx, rdx\n");
                fprintf(out, "    pop rbx\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    div rbx\n");
                fprintf(out, "    push rdx\n");
            }
            break;
        case 2: // keyword
            if (strcmp(token->value, "if") == 0) {
                fprintf(out, "    ;-- if --\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    test rax, rax\n");
                fprintf(out, "    jz addr_end_%d\n", i);
            } else if (strcmp(token->value, "ifel") == 0) {
                fprintf(out, "    ;-- if-else --\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    test rax, rax\n");
                fprintf(out, "    jz addr_else_%d\n", i);
            } else if (strcmp(token->value, ":") == 0) {
                fprintf(out, "    jmp addr_end_%d\n", i);
                fprintf(out, "    ;-- else --\n");
                fprintf(out, "addr_else_%d:\n", i);
            } else if (strcmp(token->value, "end") == 0) {
                fprintf(out, "    ;-- end --\n");
                fprintf(out, "addr_end_%d:\n", i);
                ++i;
            // } else if (strcmp(token->value, "while") == 0) {
            //     fprintf(out, "    ;-- while --\n");
            //     fprintf(out, "addr_while_%d:\n", i);
            // } else if (strcmp(token->value, "do") == 0) {
            //     fprintf(out, "    ;-- do --\n");
            //     fprintf(out, "    pop rax\n");
            //     fprintf(out, "    test rax, rax\n");
            //     fprintf(out, "    jz addr_while_%d\n", i);
            //     ++i;
            } else if (strcmp(token->value, "shr") == 0) {
                fprintf(out, "    ;-- shr --\n");
                fprintf(out, "    pop rcx\n");
                fprintf(out, "    pop rbx\n");
                fprintf(out, "    shr rbx, cl\n");
                fprintf(out, "    push rbx\n");
            } else if (strcmp(token->value, "shl") == 0) {
                fprintf(out, "    ;-- shl --\n");
                fprintf(out, "    pop rcx\n");
                fprintf(out, "    pop rbx\n");
                fprintf(out, "    shl rbx, cl\n");
                fprintf(out, "    push rbx\n");
            } else if (strcmp(token->value, "and") == 0) {
                fprintf(out, "    ;-- and --\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    pop rbx\n");
                fprintf(out, "    and rbx, rax\n");
                fprintf(out, "    push rbx\n");
            } else if (strcmp(token->value, "or") == 0) {
                fprintf(out, "    ;-- or --\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    pop rbx\n");
                fprintf(out, "    or rbx, rax\n");
                fprintf(out, "    push rbx\n");
            } else if (strcmp(token->value, "not") == 0) {
                fprintf(out, "    ;-- not --\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    not rax\n");
                fprintf(out, "    push rax\n");
            } else if (strcmp(token->value, "prn") == 0) {
                fprintf(out, "    ;-- print --\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    call printNum\n");
            } else if (strcmp(token->value, "dup") == 0) {
                fprintf(out, "    ;-- duplicate --\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    push rax\n");
                fprintf(out, "    push rax\n");
            } else if (strcmp(token->value, "swap") == 0) {
                fprintf(out, "    ;-- swap --\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    pop rbx\n");
                fprintf(out, "    push rax\n");
                fprintf(out, "    push rbx\n");
            } else if (strcmp(token->value, "drop") == 0) {
                fprintf(out, "    ;-- drop --\n");
                fprintf(out, "    pop rax\n");
            } else if (strcmp(token->value, "over") == 0) {
                fprintf(out, "    ;-- over --\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    pop rbx\n");
                fprintf(out, "    push rbx\n");
                fprintf(out, "    push rax\n");
                fprintf(out, "    push rbx\n");
            } else if (strcmp(token->value, "rot") == 0) {
                fprintf(out, "    ;-- rotate--\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    pop rbx\n");
                fprintf(out, "    pop rcx\n");
                fprintf(out, "    push rbx\n");
                fprintf(out, "    push rax\n");
                fprintf(out, "    push rcx\n");
            } else if (strcmp(token->value, "mem") == 0) {
                fprintf(out, "    push bit\n");
            } else if (strcmp(token->value, "load") == 0) {
                fprintf(out, "    ;-- load --\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    xor rbx, rbx\n");
                fprintf(out, "    mov bl, [rax]\n");
                fprintf(out, "    push rbx\n");
            } else if (strcmp(token->value, "store") == 0) {
                fprintf(out, "    ;-- store --\n");
                fprintf(out, "    pop rbx\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    mov [rax], bl\n");
            } else if (strcmp(token->value, "load64") == 0) {
                fprintf(out, "    ;-- load64 --\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    xor rbx, rbx\n");
                fprintf(out, "    mov rbx, [rax]\n");
                fprintf(out, "    push rbx\n");
            } else if (strcmp(token->value, "store64") == 0) {
                fprintf(out, "    ;-- store64 --\n");
                fprintf(out, "    pop rbx\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    mov [rax], rbx\n");
            } else if (strcmp(token->value, "argc") == 0) {
                fprintf(out, "    ;-- argc --\n");
                fprintf(out, "    mov rax, [args_ptr]\n");
                fprintf(out, "    mov rax, [rax]\n");
                fprintf(out, "    push rax\n");
            } else if (strcmp(token->value, "argv") == 0) {
                fprintf(out, "    ;-- argv --\n");
                fprintf(out, "    mov rax, [args_ptr]\n");
                fprintf(out, "    add rax, 8\n");
                fprintf(out, "    push rax\n");
            } else if (strcmp(token->value, "=") == 0) {
                fprintf(out, "    ;-- eq --\n");
                fprintf(out, "    mov rcx, 0\n");
                fprintf(out, "    mov rdx, 1\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    pop rbx\n");
                fprintf(out, "    cmp rax, rbx\n");
                fprintf(out, "    cmove rcx, rdx\n");
                fprintf(out, "    push rcx\n");
            } else if (strcmp(token->value, "!=") == 0) {
                fprintf(out, "    ;-- ne --\n");
                fprintf(out, "    mov rcx, 0\n");
                fprintf(out, "    mov rdx, 1\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    pop rbx\n");
                fprintf(out, "    cmp rax, rbx\n");
                fprintf(out, "    cmovne rcx, rdx\n");
                fprintf(out, "    push rcx\n");
            } else if (strcmp(token->value, "<") == 0) {
                fprintf(out, "    ;-- lt --\n");
                fprintf(out, "    mov rcx, 0\n");
                fprintf(out, "    mov rdx, 1\n");
                fprintf(out, "    pop rbx\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    cmp rax, rbx\n");
                fprintf(out, "    cmovl rcx, rdx\n");
                fprintf(out, "    push rcx\n");
            } else if (strcmp(token->value, "<=") == 0) {
                fprintf(out, "    ;-- lte --\n");
                fprintf(out, "    mov rcx, 0\n");
                fprintf(out, "    mov rdx, 1\n");
                fprintf(out, "    pop rbx\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    cmp rax, rbx\n");
                fprintf(out, "    cmovle rcx, rdx\n");
                fprintf(out, "    push rcx\n");
            } else if (strcmp(token->value, "!<") == 0) {
                fprintf(out, "    ;-- nlt --\n");
                fprintf(out, "    mov rcx, 0\n");
                fprintf(out, "    mov rdx, 1\n");
                fprintf(out, "    pop rbx\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    cmp rax, rbx\n");
                fprintf(out, "    cmovnl rcx, rdx\n");
                fprintf(out, "    push rcx\n");
            } else if (strcmp(token->value, ">") == 0) {
                fprintf(out, "    ;-- gt --\n");
                fprintf(out, "    mov rcx, 0\n");
                fprintf(out, "    mov rdx, 1\n");
                fprintf(out, "    pop rbx\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    cmp rax, rbx\n");
                fprintf(out, "    cmovg rcx, rdx\n");
                fprintf(out, "    push rcx\n");
            } else if (strcmp(token->value, ">=") == 0) {
                fprintf(out, "    ;-- gte --\n");
                fprintf(out, "    mov rcx, 0\n");
                fprintf(out, "    mov rdx, 1\n");
                fprintf(out, "    pop rbx\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    cmp rax, rbx\n");
                fprintf(out, "    cmovge rcx, rdx\n");
                fprintf(out, "    push rcx\n");
            } else if (strcmp(token->value, "!>") == 0) {
                fprintf(out, "    ;-- ngt --\n");
                fprintf(out, "    mov rcx, 0\n");
                fprintf(out, "    mov rdx, 1\n");
                fprintf(out, "    pop rbx\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    cmp rax, rbx\n");
                fprintf(out, "    cmovng rcx, rdx\n");
                fprintf(out, "    push rcx\n");
            } else if (strcmp(token->value, "syscall0") == 0) {
                fprintf(out, "    ;-- syscall0 --\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    syscall\n");
                fprintf(out, "    push rax\n");
            } else if (strcmp(token->value, "syscall1") == 0) {
                fprintf(out, "    ;-- syscall1 --\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    pop rdi\n");
                fprintf(out, "    syscall\n");
                fprintf(out, "    push rax\n");
            } else if (strcmp(token->value, "syscall2") == 0) {
                fprintf(out, "    ;-- syscall2 --\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    pop rdi\n");
                fprintf(out, "    pop rsi\n");
                fprintf(out, "    syscall\n");
                fprintf(out, "    push rax\n");
            } else if (strcmp(token->value, "syscall3") == 0) {
                fprintf(out, "    ;-- syscall3 --\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    pop rdi\n");
                fprintf(out, "    pop rsi\n");
                fprintf(out, "    pop rdx\n");
                fprintf(out, "    syscall\n");
                fprintf(out, "    push rax\n");
            } else if (strcmp(token->value, "syscall4") == 0) {
                fprintf(out, "    ;-- syscall4 --\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    pop rdi\n");
                fprintf(out, "    pop rsi\n");
                fprintf(out, "    pop rdx\n");
                fprintf(out, "    pop r10\n");
                fprintf(out, "    syscall\n");
                fprintf(out, "    push rax\n");
            } else if (strcmp(token->value, "syscall5") == 0) {
                fprintf(out, "    ;-- syscall5 --\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    pop rdi\n");
                fprintf(out, "    pop rsi\n");
                fprintf(out, "    pop rdx\n");
                fprintf(out, "    pop r10\n");
                fprintf(out, "    pop r8\n");
                fprintf(out, "    syscall\n");
                fprintf(out, "    push rax\n");
            } else if (strcmp(token->value, "syscall6") == 0) {
                fprintf(out, "    ;-- syscall6 --\n");
                fprintf(out, "    pop rax\n");
                fprintf(out, "    pop rdi\n");
                fprintf(out, "    pop rsi\n");
                fprintf(out, "    pop rdx\n");
                fprintf(out, "    pop r10\n");
                fprintf(out, "    pop r8\n");
                fprintf(out, "    pop r9\n");
                fprintf(out, "    syscall\n");
                fprintf(out, "    push rax\n");
            } else if (strcmp(token->value, "exit") == 0) {
                fprintf(out, "    ;-- exit --\n");
                fprintf(out, "    mov rax, 60\n");
                fprintf(out, "    mov rdi, 0\n");
                fprintf(out, "    syscall\n");
            }
            break;
        case 3: // comment
            // Skip comments
            break;
    }
}

void compile(const char *in_path, const char *out_path) {
    FILE *out = fopen(ASM_FILE, "w");
    if (!out) {
        perror("Failed to open output file");
        exit(EXIT_FAILURE);
    }

    FILE *in = fopen(in_path, "r");
    if (!in) {
        perror("Failed to open input file");
        exit(EXIT_FAILURE);
    }

    fprintf(out, "BITS 64\n");
    fprintf(out, "section .bss\n");
    fprintf(out, "    num resd 1\n");
    fprintf(out, "    args_ptr: resq 1\n");
    fprintf(out, "    bit resb %d\n", MEM_CAP);
    fprintf(out, "    digitSpace resb 64\n");
    fprintf(out, "    digitSpacePos resq 1\n");
    fprintf(out, "section .text\n");
    fprintf(out, "printNum:\n");
    fprintf(out, "    mov rcx, digitSpace\n");
    fprintf(out, "    mov rbx, 10\n");
    fprintf(out, "    mov [rcx], rbx\n");
    fprintf(out, "    inc rcx\n");
    fprintf(out, "    mov [digitSpacePos], rcx\n");
    fprintf(out, "printNumLoop:\n");
    fprintf(out, "    mov rdx, 0\n");
    fprintf(out, "    mov rbx, 10\n");
    fprintf(out, "    div rbx\n");
    fprintf(out, "    push rax\n");
    fprintf(out, "    add rdx, 48\n");
    fprintf(out, "    mov rcx, [digitSpacePos]\n");
    fprintf(out, "    mov [rcx], dl\n");
    fprintf(out, "    inc rcx\n");
    fprintf(out, "    mov [digitSpacePos], rcx\n");
    fprintf(out, "    pop rax\n");
    fprintf(out, "    cmp rax, 0\n");
    fprintf(out, "    jne printNumLoop\n");
    fprintf(out, "printNumLoop2:\n");
    fprintf(out, "    mov rcx, [digitSpacePos]\n");
    fprintf(out, "    mov rax, 1\n");
    fprintf(out, "    mov rdi, 1\n");
    fprintf(out, "    mov rsi, rcx\n");
    fprintf(out, "    mov rdx, 1\n");
    fprintf(out, "    syscall\n");
    fprintf(out, "    mov rcx, [digitSpacePos]\n");
    fprintf(out, "    dec rcx\n");
    fprintf(out, "    mov [digitSpacePos], rcx\n");
    fprintf(out, "    cmp rcx, digitSpace\n");
    fprintf(out, "    jge printNumLoop2\n");
    fprintf(out, "    ret\n");
    fprintf(out, "global _start\n");
    fprintf(out, "_start:\n");
    fprintf(out, "    mov [args_ptr], rsp\n");
    char line[LINE_SIZE];
    while (fgets(line, sizeof(line), in)) {
        Token *tokens = tokenize(line);
        for (int i = 0; tokens[i].value != NULL; ++i) {
            generate_asm(&tokens[i], out);
            free(tokens[i].value);
        }
        free(tokens);
    }

    fclose(in);
    fclose(out);

    // Assemble the code using nasm
    char nasm_command[256];
    snprintf(nasm_command, sizeof(nasm_command), "nasm -f elf64 %s -o out.o", ASM_FILE);
    int nasm_result = system(nasm_command);
    if (nasm_result != 0) {
        fprintf(stderr, "Error assembling code with nasm\n");
        exit(EXIT_FAILURE);
    }

    // Link the object file using ld
    char ld_command[256];
    snprintf(ld_command, sizeof(ld_command), "ld out.o -o %s", out_path);
    int ld_result = system(ld_command);
    if (ld_result != 0) {
        fprintf(stderr, "Error linking code with ld\n");
        exit(EXIT_FAILURE);
    }

    printf("Compilation and linking successful. Executable: %s\n", out_path);
}

int main(int argc, char *argv[]) {

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    compile(argv[1], argv[2]);

    return EXIT_SUCCESS;

}
