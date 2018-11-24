section .data                    	; data section, read-write
        an:    DQ 0              	; this is a temporary var

section .text                    	; our code is always in the .text section
        global do_Str          	    ; makes the function appear in global scope
        extern printf            	; tell linker that printf is defined elsewhere 							; (not used in the program)

do_Str:                        	    ; functions are defined as labels
        push    rbp              	; save Base Pointer (bp) original value
        mov     rbp, rsp         	; use base pointer to access stack contents
        mov rcx, rdi			    ; get function argument

;;;;;;;;;;;;;;;; FUNCTION EFFECTIVE CODE STARTS HERE ;;;;;;;;;;;;;;;; 

	mov	qword [an], 0		; initialize answer
	label_here:
                mov al,[rcx]
   		; Your code goes somewhere around here...
		cmp al,'a'    
		jl label_here_2     
		cmp al,'z'   
		jg label_here_2     
		sub al,32	    
	label_here_2:
		cmp al,'('    
		jne label_here_3    
		add al,20         
	label_here_3:
		cmp al,')'    
		jne label_here_4    
		add al,21          
	label_here_4:
		cmp al,'A'    
		jge label_here_5    
		inc byte [an]
	label_here_5:
		cmp al,'Z'   
		jle label_here_6    
		inc byte [an]
	label_here_6:
                mov [rcx],al
 
		



		inc rcx      	    ; increment pointer
		cmp byte [rcx], 0   ; check if byte pointed to is zero
		jnz label_here      ; keep looping until it is null terminated


;;;;;;;;;;;;;;;; FUNCTION EFFECTIVE CODE ENDS HERE ;;;;;;;;;;;;;;;; 

         mov     rax,[an]         ; return an (returned values are in rax)
         mov     rsp, rbp
         pop     rbp
         ret 
		 
