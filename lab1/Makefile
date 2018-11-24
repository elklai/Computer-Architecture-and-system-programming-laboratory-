all:wcounter 

wcounter : wordcounter.o 
		gcc -g -Wall -o wcounter wordcounter.o

wordcounter.o : wordcounter.c 
	gcc -g -Wall -c -o wordcounter.o wordcounter.c 

clean :
	rm wcounter wordcounter.o