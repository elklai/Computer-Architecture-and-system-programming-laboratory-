all:edit 

edit : main[1].o toy_printf.o  
		gcc -g -Wall -o edit main[1].o toy_printf.o 

main.o : main[1].c 
	gcc -g -Wall -c -o main[1].o main[1].c 
toy_printf.o : toy_printf.c toy_stdio.h
	gcc -g -Wall -c -o toy_printf.o toy_printf.c
clean :
	rm edit main[1].o toy_printf.o