client: line_parser.o client.o 
	gcc -g -Wall -o client line_parser.o client.o 
line_parser.o: line_parser.c line_parser.h 
	gcc -g -Wall -c -o line_parser.o line_parser.c
client.o: client.c 
	gcc -g -Wall -c -o client.o client.c
clean:
	rm server line_parser.o client.o 
