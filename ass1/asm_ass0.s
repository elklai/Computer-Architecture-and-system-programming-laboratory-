
section .text                    	; our code is always in the .text section
        global _asm_add          	    ; makes the function appear in global scope
	global _asm_sub
	global _asm_mult
	global _div         
	extern printf            	; tell linker that printf is defined elsewhere  
	 _asm_add:
		push rbp;
		mov rbp,rsp
		mov rax,rdi
		mov r8,rsi
		add rax,r8
		mov rsp,rbp
		pop rbp
		ret
	_asm_sub:
		push rbp;
		mov rbp,rsp
		mov rax,rdi
		mov r8,rsi
		sub rax,r8
		mov rsp,rbp
		pop rbp
		ret
	_asm_mult:
		push rbp;
		mov rbp,rsp
		mov rax,rdi
		mov r8,rsi
		mul r8
		mov rsp,rbp
		pop rbp
		ret
