import sys, os, re, subprocess
from enum import Enum, auto
from dataclasses import dataclass
from typing import *

class OpType(Enum):
	OP_PUSH_INT = auto()
	OP_PUSH_STRING = auto()
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
	OP_MACRO = auto()
	OP_CLOSE = auto()

	OP_NGT = auto()
	OP_GT = auto()
	OP_GTE = auto()
	OP_NLT = auto()
	OP_LT = auto()
	OP_LTE = auto()
	OP_NEQ = auto()
	OP_EQ = auto()
	OP_LNOT = auto()

	OP_SHR = auto()
	OP_SHL = auto()

	OP_BNOT = auto()
	OP_BOR = auto()
	OP_BXOR = auto()
	OP_BAND = auto()

	OP_DUP = auto()
	OP_SWAP = auto()
	OP_OVER = auto()
	OP_DROP = auto()

	OP_PTR = auto()
	OP_STORE = auto()
	OP_LOAD = auto()

	OP_SYSCALL1 = auto()
	OP_SYSCALL2 = auto()
	OP_SYSCALL3 = auto()
	OP_SYSCALL4 = auto()
	OP_SYSCALL5 = auto()
	OP_SYSCALL6 = auto()

	OP_PRINT_INT = auto()

@dataclass
class Op:
    typ: OpType
    value: Optional[Union[int, str]] = None
    jmp: Optional[int] = None

Program = List[Op]
macros = {}

class TokenType(Enum):
	INT=auto()
	WORD=auto()
	STR=auto()

@dataclass   
class Token():
	typ: TokenType
	value: Optional[Union[int, str]] = None

MEM_CAP = 640_000

def compile(program, out):
	strs=[]
	with open(out, "w") as out:
		out.write("BITS 64\n")
		out.write("global _start\n")
		out.write("_start:\n")

		if_label_count = 0
		while_label_count = 0
		else_stack = []
		wasElse = False
		end_stack = []
		while_stack = []
		do_stack = []
		end_while_stack = []
		sindex = 0

		for op in program:
			if op.typ == OpType.OP_PUSH_INT:
				out.write("    ; -- push int --\n")
				if op.value is not None:
					out.write(f"    push {op.value}\n")
				else:
					raise ValueError("OP_PUSH_INT operation missing value")
			elif op.typ == OpType.OP_PUSH_STRING:
				out.write(f"    ; -- push string --\n")
				if op.value is not None:
					strs.append(op.value)
					str_index = len(strs) - 1
					out.write(f"    mov rax, {len(op.value)}\n")
					out.write("    push rax\n")
					out.write(f"    push str_{sindex}\n")
					sindex += 1
				else:
					raise ValueError("OP_PUSH_STRING operation missing value")
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
				wasElse = False
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
				wasElse = True
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
				if wasElse == False:
					out.write("%s:\n" % current_else_label)
				out.write("    ; -- end --\n")
				out.write("%s:\n" % current_end_label)
				wasElse = False
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
			elif op.typ == OpType.OP_LNOT:
				out.write("    ; -- not --\n")
				out.write("    pop rax\n")
				out.write("    not rax\n")
				out.write("    push rax\n")
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
			elif op.typ == OpType.OP_BNOT:
				out.write("    mov rcx, 1\n")
				out.write("    mov rdx, 0\n")
				out.write("    pop rax\n")
				out.write("    cmp rax, 1\n")
				out.write("    cmove rcx, rdx\n")
				out.write("    push rcx\n")
			elif op.typ == OpType.OP_BOR:
				out.write("    ; -- or --\n")
				out.write("    pop rax\n")
				out.write("    pop rbx\n")
				out.write("    or rbx, rax\n")
				out.write("    push rbx\n")
			elif op.typ == OpType.OP_BXOR:
				out.write("    ; -- xor --\n")
				out.write("    pop rax\n")
				out.write("    pop rbx\n")
				out.write("    xor rbx, rax\n")
				out.write("    push rbx\n")
			elif op.typ == OpType.OP_BAND:
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
				out.write("    ; -- pop --\n")
				out.write("    pop rax\n")
			elif op.typ == OpType.OP_PTR:
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
			else:
				raise ValueError(f"Unknown opcode: {op.typ}")

		out.write("section .bss\n")
		out.write("mem resb %d\n" % MEM_CAP)
		out.write("digitSpace resb 64\n")
		out.write("digitSpacePos resq 1\n")
		out.write("section .data\n")
		for index, s in enumerate(strs):
			out.write("str_%d: db %s\n" % (index, ','.join(map(hex, list(bytes(s, 'utf-8'))))))
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
		out.write("printStr:\n")
		out.write("    pop rsi\n")
		out.write("    mov rdx, 0\n")
		out.write("countStrLen:\n")
		out.write("    cmp byte [rsi + rdx], 0\n")
		out.write("    je doneCountStrLen\n")
		out.write("    inc rdx\n")
		out.write("    jmp countStrLen\n")
		out.write("doneCountStrLen:\n")
		out.write("    mov rax, 1\n")
		out.write("    mov rdi, 1\n")
		out.write("    syscall\n")
		out.write("    ret\n")

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
	"macro": OpType.OP_MACRO,
	"close": OpType.OP_CLOSE,

	">": OpType.OP_GT,
	">=": OpType.OP_GTE,
	"!>": OpType.OP_NGT,
	"<": OpType.OP_LT,
	"<=": OpType.OP_LTE,
	"!<": OpType.OP_NLT,
	"=": OpType.OP_EQ,
	"!=": OpType.OP_NEQ,
	"!": OpType.OP_LNOT,

	"shr": OpType.OP_SHR,
	"shl": OpType.OP_SHL,

	"not": OpType.OP_BNOT,
	"or": OpType.OP_BOR,
	"xor": OpType.OP_BXOR,
	"and": OpType.OP_BAND,

	"dup": OpType.OP_DUP,
	"swap": OpType.OP_SWAP,
	"over": OpType.OP_OVER,
	"drop": OpType.OP_DROP,

	"ptr": OpType.OP_PTR,
	"store": OpType.OP_STORE,
	"load": OpType.OP_LOAD,

	"syscall1": OpType.OP_SYSCALL1,
	"syscall2": OpType.OP_SYSCALL2,
	"syscall3": OpType.OP_SYSCALL3,
	"syscall4": OpType.OP_SYSCALL4,
	"syscall5": OpType.OP_SYSCALL5,
	"syscall6": OpType.OP_SYSCALL6,

	"prn": OpType.OP_PRINT_INT,
}

def tokenize(lines: List[str], current_file: str = "") -> List[Token]:
	out = []

	for line in lines:
		words = line.split()
		for word in words:
			if word.isnumeric():
				out.append(Token(TokenType.INT, int(word)))
			elif word.startswith(";"):
				include_file = word[1:]
				print(f"Tokenizing {include_file}...")
				print(f"Parsing {include_file}...")
				if current_file:
					include_file = os.path.join(os.path.dirname(current_file), include_file)
				with open(include_file) as f:
					include_lines = f.readlines()
				include_tokens = tokenize(include_lines, include_file)
				out.extend(include_tokens)
			elif word.startswith("\""):
				word = word[1:-1]
				print(f"{word}")
				out.append(Token(TokenType.STR, str(word)))
			elif word == '//':
				break
			else:
				out.append(Token(TokenType.WORD, word))
	return out

def parse(tokens: List[Token]) -> Program:
    out = []
    macros = {}
    i = 0

    while i < len(tokens):
        token = tokens[i]
        if token.typ == TokenType.INT:
            out.append(Op(OpType.OP_PUSH_INT, token.value))
        elif token.typ == TokenType.STR:
            out.append(Op(OpType.OP_PUSH_STRING, token.value))
        elif token.typ == TokenType.WORD:
            if token.value in TOKEN_WORDS:
                op_type = TOKEN_WORDS[token.value]
                if op_type == OpType.OP_MACRO:
                    macro_name = tokens[i + 1].value
                    i += 2
                    macro_body = []
                    while tokens[i].value != 'close':
                        if tokens[i].typ == TokenType.INT:
                            macro_body.append(Op(OpType.OP_PUSH_INT, tokens[i].value))
                        elif tokens[i].typ == TokenType.STR:
                            macro_body.append(Op(OpType.OP_PUSH_STRING, tokens[i].value))
                        elif tokens[i].typ == TokenType.WORD:
                            if tokens[i].value in TOKEN_WORDS:
                                macro_body.append(Op(TOKEN_WORDS[tokens[i].value]))
                            else:
                                # Check if it's a macro
                                if tokens[i].value in macros:
                                    macro_body.extend(macros[tokens[i].value])
                                else:
                                    raise ValueError(f"Unrecognized token in macro: {tokens[i].value}")
                        i += 1
                    macros[macro_name] = macro_body
                else:
                    out.append(Op(op_type))
            else:
                if token.value in macros:
                    out.extend(macros[token.value])
                else:
                    raise ValueError(f"Unrecognized token: {token.value}")
        else:
            raise ValueError(f"Unrecognized word: {token.value}")
        i += 1

    processed_program = [op for op in out if op.typ != OpType.OP_MACRO and op.typ != OpType.OP_CLOSE]

    return processed_program

def usage():
	print("Usage: %s <flag> <in> <out>\n")
	print("flags:      -c: Compile program\n")
	print("flags:      -r: Compile program\n")

def command(cmd):
	print("[CMD] %s" % cmd)
	subprocess.call(cmd)

def main(src, out):
    with open(src) as f:
        lines = f.readlines()
    print(f"Tokenizing {afl}...")
    tokens = tokenize(lines, src)
    print(f"Parsing {afl}...")
    program = parse(tokens)
    print("----------------------------------------------------")
    compile(program, out)

if __name__ == '__main__':

	flag = sys.argv[1]
	afl = sys.argv[2]
	out = sys.argv[3]	

	if flag == "-c" or flag == "-r":
		main(f"{afl}", f"{out}.s")
		command(["nasm", "-felf64", "-o", f"{out}.o", f"{out}.s"])
		command(["ld", "-o", f"{out}", f"{out}.o"])
		if flag == "-r":
			command([f"./{out}"])
	else:
		usage()
		print("ERROR: unknown flag %s" % flag)
		exit(1)
