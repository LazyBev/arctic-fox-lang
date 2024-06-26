#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define STACK_DEPTH 1000
#define WORD_LENGTH 100

bool output_allowed = false;

int stack[STACK_DEPTH];
int stack_pointer = 0;

void push(int value) {
    if (stack_pointer < STACK_DEPTH) {
        stack[stack_pointer++] = value;
    } else {
        printf("Logic Error: Stack overflow\n");
        exit(1);
    }
}

int pop() {
    if (stack_pointer > 0) {
        return stack[--stack_pointer];
    } else {
        printf("Logic Error: Stack underflow\n");
        exit(1);
    }
}

void interpret_program(char *word, FILE *file) {
    int a, b;
    if (strcmp(word, "+") == 0) {
        a = pop();
        b = pop();
        push(b + a);
    } else if (strcmp(word, "-") == 0) {
        a = pop();
        b = pop();
        push(b - a);
    } else if (strcmp(word, "*") == 0) {
        a = pop();
        b = pop();
        push(b * a);
    } else if (strcmp(word, "/") == 0) {
        a = pop();
        b = pop();
        if (a != 0) {
            push(b / a);
        } else {
            printf("Logic Error: Division by zero\n");
            exit(1);
        }
    } else if (strcmp(word, "%") == 0) {
        a = pop();
        b = pop();
        if (a != 0) {
            push(b % a);
        } else {
            printf("Logic Error: Modulo by zero\n");
            exit(1);
        }
    } else if (strcmp(word, ".") == 0) {
        if (output_allowed) {
            printf("%d\n", pop());
        } else {
            printf("Syntax Error: @out not included, cannot output stack\n");
            exit(1);
        }
    } else if (strncmp(word, "@", 1) == 0) {
        if (strcmp(word + 1, "out") == 0) {
            output_allowed = true;
        } else {
            printf("Syntax Error: Unknown @ directive: %s\n", word);
            exit(1);
        }
    } else if (strcmp(word, "exit") == 0) {
        return;
    } else if (strncmp(word, ":", 1) == 0) {
        if (strcmp(word + 1, "dup") == 0) {
            if (stack_pointer > 0) {
                a = pop();
                push(a);
                push(a);
            } else {
                printf("Syntax Error: Stack underflow for dup\n");
                exit(1);
            }
        } else {
            push(atoi(word + 1));
        }
    } else if (strcmp(word, "dup") == 0) {
    } else if (strcmp(word, "=") == 0) {
        if (stack_pointer >= 2) {
            a = pop();
            b = pop();
            push(b == a ? 1 : 0);
        } else {
            printf("Syntax Error: Insufficient operands for =\n");
            exit(1);
        }
    } else if (strcmp(word, ">") == 0) {
        if (stack_pointer >= 2) {
            a = pop();
            b = pop();
            push(b > a ? 1 : 0);
        } else {
            printf("Syntax Error: Insufficient operands for >\n");
            exit(1);
        }
    } else if (strcmp(word, "<") == 0) {
        if (stack_pointer >= 2) {
            a = pop();
            b = pop();
            push(b < a ? 1 : 0);
        } else {
            printf("Syntax Error: Insufficient operands for <\n");
            exit(1);
        }
    } else if (strcmp(word, "!>") == 0) {
        if (stack_pointer >= 2) {
            a = pop();
            b = pop();
            push(b > a ? 0 : 1);
        } else {
            printf("Syntax Error: Insufficient operands for !>\n");
            exit(1);
        }
    } else if (strcmp(word, "!<") == 0) {
        if (stack_pointer >= 2) {
            a = pop();
            b = pop();
            push(b < a ? 0 : 1);
        } else {
            printf("Syntax Error: Insufficient operands for !<\n");
            exit(1);
        }
    } else if (strcmp(word, ">=") == 0) {
        if (stack_pointer >= 2) {
            a = pop();
            b = pop();
            push(b >= a ? 1 : 0);
        } else {
            printf("Syntax Error: Insufficient operands for >=\n");
            exit(1);
        }
    } else if (strcmp(word, "<=") == 0) {
        if (stack_pointer >= 2) {
            a = pop();
            b = pop();
            push(b <= a ? 1 : 0);
        } else {
            printf("Syntax Error: Insufficient operands for <=\n");
            exit(1);
        }
    } else if (strcmp(word, "!=") == 0) {
        if (stack_pointer >= 2) {
            a = pop();
            b = pop();
            push(b != a ? 1 : 0);
        } else {
            printf("Syntax Error: Insufficient operands for !=\n");
            exit(1);
        }
    } else if (strcmp(word, "?") == 0) {
        int condition = pop();
        if (condition == 0) {
            int depth = 1;
            while (depth > 0 && fscanf(file, "%s", word) == 1) {
                if (strcmp(word, "?") == 0) {
                    depth++;
                } else if (strcmp(word, ";") == 0) {
                    depth--;
                }
            }
        }
    } else if (strcmp(word, ";") == 0) {
    } else if (strcmp(word, "#") == 0) {
        char c;
        while ((c = fgetc(file)) != '\n' && c != EOF) {
        }
    } else {
        printf("Syntax Error: Invalid token: %s\n", word);
        exit(1);
    }
}

void interpret_file(FILE *file) {
    char word[WORD_LENGTH];
    bool exit_found = false;

    while (fscanf(file, "%s", word) == 1) {
        interpret_program(word, file);
        if (strcmp(word, "exit") == 0) {
            exit_found = true;
            break;
        }
    }

    if (!exit_found) {
        printf("Error: 'exit' not found at the end of file\n");
        exit(1);
    }
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <file>.afl\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", argv[1]);
        return 1;
    }

    interpret_file(file);
    fclose(file);

    return 0;
}
