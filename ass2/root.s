section .data
output:
	dq "root = %.16e %.16e",10, 0
outputResult:
	dq "the result for the polinom is: %lf %lf",10, 0
outputAdd:
	dq "real is: %lf ,img is: %lf",10, 0
outputOrder:
	dq "%ld",10, 0
epsilonOutput: 
	dq "epsilon = %e " ,10,0
outputarray:
	dq "number 0  : %lf %lf number 1  : %lf %lf number 2 : %lf %lf number 3 : %lf %lf ",10, 0
outputDerived:
	dq "number 0  : %lf %lf number 1  : %lf %lf number 2 : %lf %lf ",10, 0
input_string_epsilon: 
	dq "epsilon = %lf \n" ,0
input_string_order:
	dq "order = %ld \n",0
input_string_coeff:
	dq "coeff %ld = %lf %lf \n",0
input_string_initial:
	dq "initial = %lf %lf",0
        tmp:	DQ 0.0	
        i: DQ 0




section .bss

	epsilon: resq 1
	tolerance_level: resq 1
	some_of_ower_space: resq 2
	some_of_ower_space2: resq 2
	some_of_ower_space3: resq 2
	tmp_mult: resq 1
	tmp_spuare: resq 1
	tmp_polinom: resq 2
	order: resq 1
	loop_count: resq 1
	real_D_polinom: resq 1 
	imp_D_polinom: resq 1
	coeff_real: resq 1
	coeff_img: resq 1
	j: resq 1
	result: resq 2
	z_root: resq 2
	coeff_array_polinom: resq 2
	something: rest 100
	coeff_D_array_polinom: resq 2

	

section .text

	global main
	global _derived
	extern printf
	extern scanf
	extern malloc
	extern free


%macro z_complex_constuctor 3
	finit
	mov r15, %1
	mov r14, %2
	mov r13, %3
	sal r15, 4
	mov qword[coeff_array_polinom+r15], r14
	add r15, 8
	mov qword[coeff_array_polinom+r15], r13

	fild %1
	ftst 
	fstsw ax
	sahf  
	jz %%done 

	fild %1
	fld %2
	fmulp st1
	mov r11,%1
	dec r11
	sal r11, 4
	fstp qword[coeff_D_array_polinom+r11]
	add r11, 8
	fild %1
	fld %3
	fmulp st1
	fstp qword[coeff_D_array_polinom+r11]
%%done:

%endmacro


%macro _add 4
        finit
	fld %1	
	fst st1	
	fld %3	
        fadd     
	fstp   %3

	fld %2	
	fst st1	
	fld %4	
	fadd	
	fstp %4
%endmacro


%macro _sub  0
        finit
	fld qword [rdi]		
	fst st1			
	fld qword [rdx]		
	fsubp st1		
	fstp qword [rdx]		

	
	fld qword [rsi]	
	fst st1		
	fld qword [rcx]	
	fsubp st1	
	fstp qword [rcx]
%endmacro


%macro _mul 8
finit
		fld %1
		fld %3
		fmulp st1
		fstp %5 
		fld %2
		fld %4
		fmulp st1
		fchs
		fstp %7
		fld %1
		fld %4
		fmulp st1
		fstp %6
		fld %3
		fld %2
		fmulp st1
		fstp %8
		fld %5
		fld %7
		faddp st1
		fstp %7 
		fld %6
		fld %8
		faddp st1
		fstp %8 


%endmacro

%macro _square 3
		finit
        fld %1
        fst st1
        fmulp st1
        fstp %3
        fld %2
        fst st1
        fmulp st1
        fld %3
        fadd st1
        fstp %3
%endmacro

%macro _div 8
finit
		fld %4
		fchs 
		fstp %7
		
		_mul %1 , %2, %3, %7, qword[ some_of_ower_space3 ], qword[ some_of_ower_space3 + 8], %5, %6
		fld %3
		fst st1
		fmulp st1
		fstp %8
		fld %4
		fst st1
		fmulp st1
		fld %8
		fadd st1
		fstp %8
		fld %8
		fld %5
		fdiv st1
		fstp %5
		fld %8
		fld %6
		fdiv st1
		fstp %6		
%endmacro


%macro _is_in_tolerance_level 1
		finit
		fld %1
		fsqrt 
		fcom qword[epsilon]
		fstsw ax; status word
		sahf ; flag 
		jnb %%false ; jump if not below
		mov r15, 1;
		jmp %%done
		 	%%false:
				mov r15,0 ; false
		%%done:
%endmacro 


%macro _z_polinum_value 1
		finit
		fldz
		fst qword[result]
		fstp qword[result+8]
		mov r14, qword[order]
		
		mov r12, r14
		sal r12, 4

		%%loop:
			cmp r14,0
			jz %%doneLoop		
_add qword[coeff_array_polinom+r12 ], qword[coeff_array_polinom+r12+8], qword[result], qword[result+8] ; result contains sum
_mul qword[result], qword[result+8], qword[z_root], qword[z_root + 8],qword[some_of_ower_space2], qword[some_of_ower_space2+8], qword[some_of_ower_space], qword[some_of_ower_space+8]
			mov r13, qword[some_of_ower_space]
			mov qword[result], r13
			mov r13, qword[some_of_ower_space+8]
			mov qword[result+8], r13
			dec r14
			sub r12, 16
			jmp %%loop

		%%doneLoop:	
%endmacro

%macro _z_polinum_d_value 1
		finit
		fldz
		fst qword[result]
		fstp qword[result+8]
		mov r14, %1
		
		mov r12, r14
		sal r12, 4

		%%loop:
			cmp r14,0
			jz %%doneLoop
_add qword[coeff_D_array_polinom+r12 ], qword[coeff_D_array_polinom+8+r12], qword[result], qword[result+8] ; result contains sum
_mul qword[result], qword[result+8], qword[z_root], qword[z_root + 8],qword[some_of_ower_space2], qword[some_of_ower_space2+8], qword[some_of_ower_space], qword[some_of_ower_space+8]
			mov r13, qword[some_of_ower_space]
			mov qword[result], r13
			mov r13, qword[some_of_ower_space+8]
			mov qword[result+8], r13
			dec r14
			sub r12, 16
			jmp %%loop

		%%doneLoop:	
%endmacro


main: 
	push rbp
	mov rbp, rsp
	finit

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;start_of_txt;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	.call_scanf_epsilon:
		lea rdi, [input_string_epsilon]
		lea rsi, [epsilon]
		call scanf

	.call_scanf_order:
		lea rdi, [input_string_order]
		lea rsi, [order]
		call scanf
	

		mov qword[loop_count], 0
		dec qword[loop_count]



	.create_array:
		mov rax, [order]
		inc rax
		sal rax, 4
		mov rdi, rax
		call malloc
		mov qword[coeff_array_polinom], rax



	.call_scanf_coeff:
		mov r8, [loop_count]
		cmp r8, [order]
		je .call_scanf_initial_z_root
		lea rdi, [input_string_coeff]
		lea rsi, [j]
		lea rdx, [coeff_real]
		lea rcx, [coeff_img]
		call scanf
		z_complex_constuctor qword[j],qword[coeff_real] ,qword[coeff_img]
		inc qword[loop_count]
		jmp .call_scanf_coeff


	 .call_scanf_initial_z_root:
		lea rdi, [input_string_initial]
		lea rsi, [z_root]
		lea rdx, [z_root+8]
		call scanf

	.mainLoop:
		_z_polinum_value qword[order]
		_add qword[coeff_array_polinom], qword[coeff_array_polinom+8], qword[result], qword[result+8]
		_square qword[result],qword[result+8], qword[tolerance_level]
		_is_in_tolerance_level qword[tolerance_level]
		cmp r15,0
		jnz .finished
		mov r10, qword[result]
		mov qword[tmp_polinom], r10
		mov r10, qword[result+8]
		mov qword[tmp_polinom+8], r10
		mov r11, [order]
		dec r11
		_z_polinum_d_value r11
		_add qword[coeff_D_array_polinom], qword[coeff_D_array_polinom+8], qword[result], qword[result+8]
		mov r10, qword[result]
		mov qword[real_D_polinom], r10
		mov r10, qword[result+8]
		mov qword[imp_D_polinom], r10
		_div qword[tmp_polinom], qword[tmp_polinom+8],qword[real_D_polinom],qword[imp_D_polinom],qword[result],qword[result+8],qword[tmp_mult],qword[tmp_spuare]
		mov rdi,z_root
		mov rsi,z_root+8
		mov rdx,result
		mov rcx,result+8
		_sub
		mov r10, qword[result]
		mov qword[z_root], r10
		mov r10, qword[result+8]
		mov qword[z_root+8], r10
		jmp .mainLoop

	.finished:
		lea rdi, [output]
		movsd xmm0, [z_root]
		movsd xmm1, [z_root+8]
		call printf

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;end_of_txt;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	mov rsp, rbp
	pop rbp
	ret



