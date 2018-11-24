all:calc 

calc : main_ass0.o asm_ass0.o 
		gcc -g -Wall -o calc main_ass0.o asm_ass0.o

main_ass0.o : main_ass0.c 
	gcc -g -Wall -c -o main_ass0.o main_ass0.c 
asm_ass0.o : asm_ass0.s 
	nasm -g -f elf64 -w+all -o asm_ass0.o asm_ass0.s
clean :
	rm calc main_ass0.o asm_ass0.o
