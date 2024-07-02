import sys
from enum import Enum, auto
import subprocess
from dataclasses import dataclass
from typing import *

class OpType(Enum):
	OP_PUSH_INT = auto()
	OP_PLUS = auto()
	OP_MINUS = auto()
	OP_MULTIPLY = auto()
	OP_DIVIDE = auto()
	OP_MODULUS = auto()

	OP_IF = auto()
	OP_ELSE = auto()
	OP_END = auto()
	OP_WHILE = auto()
	OP_DO = auto()
	OP_ENDWHILE = auto()

	OP_NGT = auto()
	OP_GT = auto()
	OP_GTE = auto()
	OP_NLT = auto()
	OP_LT = auto()
	OP_LTE = auto()
	OP_NEQ = auto()
	OP_EQ = auto()
	OP_SHR = auto()
	OP_SHL = auto()
	OP_NOT = auto()
	OP_OR = auto()
	OP_XOR = auto()
	OP_AND = auto()
	OP_DUP = auto()
	OP_2DUP = auto()
	OP_SWAP = auto()
	OP_OVER = auto()
	OP_DROP = auto()
	OP_MEM = auto()
	OP_STORE = auto()
	OP_LOAD = auto()
	OP_SYSCALL1 = auto()
	OP_SYSCALL2 = auto()
	OP_SYSCALL3 = auto()
	OP_SYSCALL4 = auto()
	OP_SYSCALL5 = auto()
	OP_SYSCALL6 = auto()
	OP_PRINT_INT = auto()
	OP_EXIT = auto()

@dataclass
class Op:
    typ: OpType
    value: Optional[Union[int, str]] = None
    jmp: Optional[int] = None

Program=List[Op]

class TokenType(Enum):
    WORD=auto()
    INT=auto()
    STR=auto()

@dataclass   
class Token():
	typ: TokenType
	value: Union[int, str]
 
MEM_CAP = 640_000

def compile(program, out):
	with open(out, "w") as out:
		out.write("BITS 64\n")
		out.write("section .bss\n")
		out.write("mem resb %d\n" % MEM_CAP)
		out.write("digitSpace resb 64\n")
		out.write("digitSpacePos resq 1\n")
		out.write("section .text\n")
		out.write("printNum:\n")
		out.write("    mov rcx, digitSpace\n")
		out.write("    mov rbx, 10\n")
		out.write("    mov [rcx], rbx\n")
		out.write("    inc rcx\n")
		out.write("    mov [digitSpacePos], rcx\n")				
		out.write("printNumLoop:\n")
		out.write("    mov rdx, 0\n")
		out.write("    mov rbx, 10\n")
		out.write("    div rbx\n")
		out.write("    push rax\n")
		out.write("    add rdx, 48\n")
		out.write("    mov rcx, [digitSpacePos]\n")
		out.write("    mov [rcx], dl\n")
		out.write("    inc rcx\n")
		out.write("    mov [digitSpacePos], rcx\n")
		out.write("    pop rax\n")
		out.write("    cmp rax, 0\n")
		out.write("    jne printNumLoop\n")
		out.write("printNumLoop2:\n")
		out.write("    mov rcx, [digitSpacePos]\n")
		out.write("    mov rax, 1\n")
		out.write("    mov rdi, 1\n")
		out.write("    mov rsi, rcx\n")
		out.write("    mov rdx, 1\n")
		out.write("    syscall\n")
		out.write("    mov rcx, [digitSpacePos]\n")
		out.write("    dec rcx\n")
		out.write("    mov [digitSpacePos], rcx\n")
		out.write("    cmp rcx, digitSpace\n")
		out.write("    jge printNumLoop2\n")
		out.write("    ret\n")
		out.write("global _start\n")
		out.write("_start:\n")

		if_label_count = 0
		while_label_count = 0
		else_stack = []
		end_stack = []
		while_stack = []
		do_stack = []
		end_while_stack = []

		for op in program:
			if op.typ == OpType.OP_PUSH_INT:
				out.write("    ; -- push int --\n")
				if op.value is not None:
					out.write("    push %d\n" % op.value)
				else:
					raise ValueError("OP_PUSH_INT operation missing value")
			elif op.typ == OpType.OP_PLUS:
				out.write("    ; -- plus --\n")
				out.write("    pop rax\n")
				out.write("    pop rbx\n")
				out.write("    add rax, rbx\n")
				out.write("    push rax\n")
			elif op.typ == OpType.OP_MINUS:
				out.write("    ; -- minus --\n")
				out.write("    pop rax\n")
				out.write("    pop rbx\n")
				out.write("    sub rbx, rax\n")
				out.write("    push rbx\n")
			elif op.typ == OpType.OP_MULTIPLY:
				out.write("    ; -- multiply --\n")
				out.write("    pop rax\n")
				out.write("    pop rbx\n")
				out.write("    imul rax, rbx\n")
				out.write("    push rax\n")
			elif op.typ == OpType.OP_DIVIDE:
				out.write("    ; -- divide --\n")
				out.write("    pop rbx\n")
				out.write("    pop rax\n")
				out.write("    xor rdx, rdx\n")
				out.write("    div rbx\n")
				out.write("    push rax\n")
			elif op.typ == OpType.OP_MODULUS:
				out.write("    ; -- modulus --\n")
				out.write("    pop rbx\n")
				out.write("    pop rax\n")
				out.write("    xor rdx, rdx\n")
				out.write("    div rbx\n")
				out.write("    push rdx\n")
			elif op.typ == OpType.OP_IF:
				if_label_count += 1
				else_label = "addr_else_%d" % if_label_count
				end_label = "addr_end_%d" % if_label_count
				else_stack.append(else_label)
				end_stack.append(end_label)
				out.write("    ; -- if --\n")
				out.write("    pop rax\n")
				out.write("    test rax, rax\n")
				out.write("    jz %s\n" % else_label)
			elif op.typ == OpType.OP_ELSE:
				current_else_label = else_stack.pop()
				current_end_label = end_stack.pop()
				end_label = "addr_end_%d" % if_label_count
				else_stack.append(current_else_label)
				end_stack.append(current_end_label)
				out.write("    jmp %s\n" % current_end_label)
				out.write("    ; -- else --\n")
				out.write("%s:\n" % current_else_label)
			elif op.typ == OpType.OP_END:
				current_else_label = else_stack.pop()
				current_end_label = end_stack.pop()
				out.write("    ; -- end --\n")
				out.write("%s:\n" % current_end_label)
			elif op.typ == OpType.OP_WHILE:
				while_label_count += 1
				while_label = "addr_while_%d" % while_label_count
				end_while_label = "addr_end_while_%d" % while_label_count
				while_stack.append(while_label)
				end_while_stack.append(end_while_label)
				out.write("    ; -- while --\n")
				out.write("%s:\n" % while_label)
			elif op.typ == OpType.OP_DO:
				current_while_label = while_stack.pop()
				current_end_while_label = end_while_stack.pop()
				while_label = "addr_while_%d" % while_label_count
				end_while_label = "addr_end_while_%d" % while_label_count
				while_stack.append(current_while_label)
				end_while_stack.append(current_end_while_label)
				out.write("    ; -- do --\n")
				out.write("    pop rax\n")
				out.write("    test rax, rax\n")
				out.write("    jz %s\n" % end_while_label)
			elif op.typ == OpType.OP_ENDWHILE:
				current_while_label = while_stack.pop()
				current_end_while_label = end_while_stack.pop()
				out.write("    ; -- end while--\n")
				out.write("    jmp %s\n" % current_while_label)
				out.write("%s:\n" % current_end_while_label)
			elif op.typ == OpType.OP_GT:
				out.write("    ; -- greater than --\n")
				out.write("    mov rcx, 0\n")
				out.write("    mov rdx, 1\n")
				out.write("    pop rax\n")
				out.write("    pop rbx\n")
				out.write("    cmp rax, rbx\n")
				out.write("    cmovg rcx, rdx\n")
				out.write("    push rcx\n")
			elif op.typ == OpType.OP_GTE:
				out.write("    ; -- greater than or equal to --\n")
				out.write("    mov rcx, 0\n")
				out.write("    mov rdx, 1\n")
				out.write("    pop rax\n")
				out.write("    pop rbx\n")
				out.write("    cmp rax, rbx\n")
				out.write("    cmovge rcx, rdx\n")
				out.write("    push rcx\n")
			elif op.typ == OpType.OP_NGT:
				out.write("    ; -- not greater than --\n")
				out.write("    mov rcx, 1\n")
				out.write("    mov rdx, 0\n")
				out.write("    pop rax\n")
				out.write("    pop rbx\n")
				out.write("    cmp rax, rbx\n")
				out.write("    cmovg rcx, rdx\n")
				out.write("    push rcx\n")
			elif op.typ == OpType.OP_LT:
				out.write("    ; -- less than --\n")
				out.write("    mov rcx, 0\n")
				out.write("    mov rdx, 1\n")
				out.write("    pop rax\n")
				out.write("    pop rbx\n")
				out.write("    cmp rax, rbx\n")
				out.write("    cmovl rcx, rdx\n")
				out.write("    push rcx\n")
			elif op.typ == OpType.OP_LTE:
				out.write("    ; -- greater than or equal to --\n")
				out.write("    mov rcx, 0\n")
				out.write("    mov rdx, 1\n")
				out.write("    pop rax\n")
				out.write("    pop rbx\n")
				out.write("    cmp rax, rbx\n")
				out.write("    cmovle rcx, rdx\n")
				out.write("    push rcx\n")
			elif op.typ == OpType.OP_NLT:
				out.write("    ; -- not less than --\n")
				out.write("    mov rcx, 1\n")
				out.write("    mov rdx, 0\n")
				out.write("    pop rax\n")
				out.write("    pop rbx\n")
				out.write("    cmp rax, rbx\n")
				out.write("    cmovl rcx, rdx\n")
				out.write("    push rcx\n")
			elif op.typ == OpType.OP_EQ:
				out.write("    ; -- equal --\n")
				out.write("    mov rcx, 0\n")
				out.write("    mov rdx, 1\n")
				out.write("    pop rax\n")
				out.write("    pop rbx\n")
				out.write("    cmp rax, rbx\n")
				out.write("    cmove rcx, rdx\n")
				out.write("    push rcx\n")
			elif op.typ == OpType.OP_NEQ:
				out.write("    ; -- not equal --\n")
				out.write("    mov rcx, 1\n")
				out.write("    mov rdx, 0\n")
				out.write("    pop rax\n")
				out.write("    pop rbx\n")
				out.write("    cmp rax, rbx\n")
				out.write("    cmove rcx, rdx\n")
				out.write("    push rcx\n")
			elif op.typ == OpType.OP_SHR:
				out.write("    ; -- shift to right --\n")
				out.write("    pop rcx\n")
				out.write("    pop rbx\n")
				out.write("    shr rbx, cl\n")
				out.write("    push rbx\n")
			elif op.typ == OpType.OP_SHL:
				out.write("    ; -- shift to left --\n")
				out.write("    pop rcx\n")
				out.write("    pop rbx\n")
				out.write("    shl rbx, cl\n")
				out.write("    push rbx\n")
			elif op.typ == OpType.OP_NOT:
				out.write("    ; -- not --\n")
				out.write("    pop rax\n")
				out.write("    not rax\n")
				out.write("    push rax\n")
			elif op.typ == OpType.OP_OR:
				out.write("    ; -- or --\n")
				out.write("    pop rax\n")
				out.write("    pop rbx\n")
				out.write("    or rbx, rax\n")
				out.write("    push rbx\n")
			elif op.typ == OpType.OP_XOR:
				out.write("    ; -- xor --\n")
				out.write("    pop rax\n")
				out.write("    pop rbx\n")
				out.write("    xor rbx, rax\n")
				out.write("    push rbx\n")
			elif op.typ == OpType.OP_AND:
				out.write("    ; -- and --\n")
				out.write("    pop rax\n")
				out.write("    pop rbx\n")
				out.write("    and rbx, rax\n")
				out.write("    push rbx\n")
			elif op.typ == OpType.OP_DUP:
				out.write("    ; -- duplicate top 1--\n")
				out.write("    pop rax\n")
				out.write("    push rax\n")
				out.write("    push rax\n")
			elif op.typ == OpType.OP_2DUP:
				out.write("    ; -- duplicate top 2--\n")
				out.write("    pop rax\n")
				out.write("    pop rbx\n")
				out.write("    push rbx\n")
				out.write("    push rax\n")
				out.write("    push rbx\n")
				out.write("    push rax\n")
			elif op.typ == OpType.OP_SWAP:
				out.write("    ; -- swap --\n")
				out.write("    pop rax\n")
				out.write("    pop rbx\n")
				out.write("    push rax\n")
				out.write("    push rbx\n")
			elif op.typ == OpType.OP_OVER:
				out.write("    ; -- over --\n")
				out.write("    pop rax\n")
				out.write("    pop rbx\n")
				out.write("    push rbx\n")
				out.write("    push rax\n")
				out.write("    push rbx\n")
			elif op.typ == OpType.OP_DROP:
				out.write("    ; -- drop --\n")
				out.write("    pop rax\n")
			elif op.typ == OpType.OP_MEM:
				out.write("    ; -- mem --\n")
				out.write("    push mem\n")
			elif op.typ == OpType.OP_STORE:
				out.write("    ; -- store --\n")
				out.write("    pop rbx\n")
				out.write("    pop rax\n")
				out.write("    mov [rax], bl\n")
			elif op.typ == OpType.OP_LOAD:
				out.write("    ; -- load --\n")
				out.write("    pop rax\n")
				out.write("    xor rbx, rbx\n")
				out.write("    mov bl, [rax]\n")
				out.write("    push rbx\n")
			elif op.typ == OpType.OP_SYSCALL1:
				out.write("    ; -- syscall --\n")
				out.write("    pop rax\n")
				out.write("    pop rdi\n")
				out.write("    syscall\n")
			elif op.typ == OpType.OP_SYSCALL2:
				out.write("    ; -- syscall --\n")
				out.write("    pop rax\n")
				out.write("    pop rdi\n")
				out.write("    pop rsi\n")
				out.write("    syscall\n")
			elif op.typ == OpType.OP_SYSCALL3:
				out.write("    ; -- syscall --\n")
				out.write("    pop rax\n")
				out.write("    pop rdi\n")
				out.write("    pop rsi\n")
				out.write("    pop rdx\n")
				out.write("    syscall\n")
			elif op.typ == OpType.OP_SYSCALL4:
				out.write("    ; -- syscall --\n")
				out.write("    pop rax\n")
				out.write("    pop rdi\n")
				out.write("    pop rsi\n")
				out.write("    pop rdx\n")
				out.write("    pop r10\n")
				out.write("    syscall\n")
			elif op.typ == OpType.OP_SYSCALL5:
				out.write("    ; -- syscall --\n")
				out.write("    pop rax\n")
				out.write("    pop rdi\n")
				out.write("    pop rsi\n")
				out.write("    pop rdx\n")
				out.write("    pop r10\n")
				out.write("    pop r8\n")
				out.write("    syscall\n")
			elif op.typ == OpType.OP_SYSCALL6:
				out.write("    ; -- syscall --\n")
				out.write("    pop rax\n")
				out.write("    pop rdi\n")
				out.write("    pop rsi\n")
				out.write("    pop rdx\n")
				out.write("    pop r10\n")
				out.write("    pop r8\n")
				out.write("    pop r9\n")
				out.write("    syscall\n")
			elif op.typ == OpType.OP_PRINT_INT:
				out.write("    ; -- print --\n")
				out.write("    pop rax\n")
				out.write("    call printNum\n")
			elif op.typ == OpType.OP_EXIT:
				out.write("    ; -- exit --\n")
				out.write("    mov rax, 60\n")
				out.write("    mov rdi, 0\n")
				out.write("    syscall\n")
			else:
				raise RuntimeError("Unsupported opcode: %d" % op.typ)

TOKEN_WORDS = {
	"+": OpType.OP_PLUS,
	"-": OpType.OP_MINUS,
	"*": OpType.OP_MULTIPLY,
	"/": OpType.OP_DIVIDE,
	"%": OpType.OP_MODULUS,
 
	"if": OpType.OP_IF,
	"else": OpType.OP_ELSE,
	"end": OpType.OP_END,
	"while": OpType.OP_WHILE,
	"do": OpType.OP_DO,
	"done": OpType.OP_ENDWHILE,
 
	">": OpType.OP_GT,
	">=": OpType.OP_GTE,
	"<": OpType.OP_LT,
	"<=": OpType.OP_LTE,
	"!=": OpType.OP_NEQ,
	"=": OpType.OP_EQ,
	"shr": OpType.OP_SHR,
	"shl": OpType.OP_SHL,
	"not": OpType.OP_NOT,
	"or": OpType.OP_OR,
	"xor": OpType.OP_XOR,
	"and": OpType.OP_AND,
	"dup": OpType.OP_DUP,
	"2dup": OpType.OP_2DUP,
	"swap": OpType.OP_SWAP,
	"over": OpType.OP_OVER,
	"drop": OpType.OP_DROP,
	"mem": OpType.OP_MEM,
	".": OpType.OP_STORE,
	",": OpType.OP_LOAD,
	"syscall1": OpType.OP_SYSCALL1,
	"syscall2": OpType.OP_SYSCALL2,
	"syscall3": OpType.OP_SYSCALL3,
	"syscall4": OpType.OP_SYSCALL4,
	"syscall5": OpType.OP_SYSCALL5,
	"syscall6": OpType.OP_SYSCALL6,
	"prn": OpType.OP_PRINT_INT,
	"exit": OpType.OP_EXIT
}

def parse_word(word):
    if word.lstrip('-').isdigit():
        return Op(typ=OpType.OP_PUSH_INT, value=int(word))
    elif word in TOKEN_WORDS:
        return Op(typ=TOKEN_WORDS[word])
    else:
        raise ValueError(f"Unknown word '{word}'")

def load_program(file_name):
    program = []
    with open(file_name, "r") as f:
        for line_num, line in enumerate(f, start=1):
            tokens = line.split()
            if tokens:
                for token in tokens:
                    try:
                        op = parse_word(token)
                        program.append(op)
                    except ValueError as e:
                        raise ValueError(f"Line {line_num}: {e}")
    return program


def usage():
	print("Usage: %s <flag> <in> <out>\n")
	print("flags:      -c: Compile program\n")
	print("flags:      -r: Compile program\n")

def command(cmd):
	print("[CMD] %s" % cmd)
	subprocess.call(cmd)

if __name__ == '__main__':

	flag = sys.argv[1]
	afl = sys.argv[2]
	out = sys.argv[3]	

	if flag == "-c" or flag == "-r":
		program = load_program(afl)
		compile(program, f"{out}.s")
		command(["nasm", "-felf64", "-o", f"{out}.o", f"{out}.s"])
		command(["ld", "-o", f"{out}", f"{out}.o"])
		if flag == "-r":
			command([f"./{out}"])
	else:
		usage()
		print("ERROR: unknown flag %s" % flag)
		exit(1)
