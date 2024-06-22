section .bss
  buffer resb 20
  digitSpace resb 100
  digitSpacePos resb 8
section .data
  newline db 0xA
section .text
global _start
_start:
  ;; -- push 60 --
  push 60
  ;; -- push 60 --
  push 60
  ;; -- equal --
  pop rax
  pop rbx
  cmp rbx, rax
  sete al
  movzx rax, al
  push rax
  ;; -- dump --
  pop rax
  call print
  ;; -- push 0 --
  push 0
  ;; -- push 0 --
  push 0
  ;; -- push 1 --
  push 1
  ;; -- push 59 --
  push 59
  ;; -- push 60 --
  push 60
  ;; -- equal --
  pop rax
  pop rbx
  cmp rbx, rax
  sete al
  movzx rax, al
  push rax
  ;; -- dump --
  pop rax
  call print
  ;; -- push 0 --
  push 0
  ;; -- push 0 --
  push 0
  ;; -- push 0 --
  push 0
  ;; -- push 20 --
  push 20
  ;; -- push 10 --
  push 10
  ;; -- minus --
  pop rax
  pop rbx
  sub rbx, rax
  push rbx
  ;; -- dump --
  pop rax
  call print
  ;; -- push 0 --
  push 0
  ;; -- push 0 --
  push 0
  ;; -- push 10 --
  push 10
  ;; -- push 20 --
  push 20
  ;; -- push 10 --
  push 10
  ;; -- plus --
  pop rax
  pop rbx
  add rax, rbx
  push rax
  ;; -- dump --
  pop rax
  call print
  ;; -- push 0 --
  push 0
  ;; -- push 0 --
  push 0
  ;; -- push 30 --
  push 30
  ;; -- push 50 --
  push 50
  ;; -- push 5 --
  push 5
  ;; -- divide --
  pop rbx
  pop rax
  xor rdx, rdx
  div rbx
  push rax
  ;; -- dump --
  pop rax
  call print
  ;; -- push 0 --
  push 0
  ;; -- push 0 --
  push 0
  ;; -- push 10 --
  push 10
  ;; -- push 59 --
  push 59
  ;; -- push 2 --
  push 2
  ;; -- modulus --
  pop rbx
  pop rax
  xor rdx, rdx
  div rbx
  push rdx
  ;; -- dump --
  pop rax
  call print
  ;; -- push 0 --
  push 0
  ;; -- push 0 --
  push 0
  ;; -- push 1 --
  push 1
  mov rax, 60
  mov rdi, 0
  syscall
print:
  mov rcx, digitSpace
  mov rbx, 10
  mov [rcx], rbx
  inc rcx
  mov [digitSpacePos], rcx
printLoop:
  mov rdx, 0
  mov rbx, 10
  div rbx
  push rax
  add rdx, 48
  mov rcx, [digitSpacePos]
  mov [rcx], dl
  inc rcx
  mov [digitSpacePos], rcx
  pop rax
  cmp rax, 0
  jne printLoop
printLoop2:
  mov rcx, [digitSpacePos]
  mov rax, 1
  mov rdi, 1
  mov rsi, rcx
  mov rdx, 1
  syscall
  mov rcx, [digitSpacePos]
  dec rcx
  mov [digitSpacePos], rcx
  cmp rcx, digitSpace
  jge printLoop2
  ret
