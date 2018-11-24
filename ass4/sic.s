	section .data
		%define MAX_PHYSICAL_MEMORY 4096
		format_int:			db "%ld", 0 ;; saves the format to print
		format_print:		db "%ld ", 0 ;; saves the format to print
		format_line:		db 10, 0 	;; saves the format of "\n"

	section .bss
		temp:				resq 1 ;; saves the input from user
		array:				resq 1 ;; saves pointer to the array of int 
		size_of_array:		resq 1 ;; size of array


	section .text

		extern printf, scanf, calloc , malloc, free, exit
		global main

		main:
				
			enter 0,0

			;;; allocated max memory for int array ;;;

			lea rdi, [MAX_PHYSICAL_MEMORY*8]	;; max size * 8 for int
			call malloc							;; allocated memory for array
			mov qword [array], rax				;; pointer to array

			mov r15, 0 ;; counter

			.reading_user_input:
			
				;;; while input isnt EOF - keep reading ;;;
				;push r10	
				mov rdi, format_int
				mov rsi, temp
				call scanf
				;pop r10
				cmp rax, 1						
				jne .end_user_input 			;; if we got EOF

				lea r11, [8*r15] 				;; puts [r10*8] into r11
				add r11, qword [array] 	;; pointer to the right place
				mov r13, qword [temp] 			;; puts in r13 the given int
				mov qword [r11], r13			;; inserts the given int to the right place
				inc r15							;; inc index
				jmp .reading_user_input

			.end_user_input:
				mov [size_of_array], r15	;; size of arary 
				mov r8, [array]					;; r15 holds the pointer to the array
				mov r9, 0 						;; i - index



			mov r8, 0
			mov r9, [array]
	
			.while_loop:
				mov r10, [r8*8+r9]			;; M[i]
				mov r11, [r8*8+8+r9]		;; M[i+1]
				mov r12, [r8*8+16+r9]		;; M[i+2]
				cmp r10, 0
				jne .while_cond_ok
				cmp r11, 0
				jne .while_cond_ok
				cmp r12, 0
				je .init_for_loop

			.while_cond_ok:
				mov r13, [r10*8+r9]			;; M[M[i]]
				mov r14, [r11*8+r9]			;; M[M[i+1]]
				sub r13, r14
				mov [r10*8+r9], r13
				cmp r13, 0
				jl .then
				add r8, 3					;; else of if
				jmp .while_loop

			.then:							;; then of if
				mov r8, r12
				jmp .while_loop

		.test2:
			;;			mov rdi, format_print
			;;			mov rsi, [r12]
			;;			mov rax, 0
			;;			call printf
		.init_for_loop:						;; initialization of for loop
			mov r15, [size_of_array]
			mov r12, [array]
			add r15, 1

		.for_body:
			cmp r15, 1
			je .end_for
			mov rdi, format_print
			mov rsi, [r12]
			mov rax, 0
			call printf
			add r12, 8
			sub r15, 1
			jmp .for_body

		.test3:
			;; testing the format line
			mov rdi, r10
			mov r10, rdi

		.end_for:
			mov rdi , format_line
			mov rax , 0
			call printf
			
			mov rdi, qword [array]				;; free allocated memory
			call free							;; free allocated memory

			leave
			ret