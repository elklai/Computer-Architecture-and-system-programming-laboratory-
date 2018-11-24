server: line_parser.o server.o common.o
	gcc -g -Wall -o server line_parser.o server.o common.o
line_parser.o: line_parser.c line_parser.h 
	gcc -g -Wall -c -o line_parser.o line_parser.c
server.o: server.c 
	gcc -g -Wall -c -o server.o server.c
common.o: common.c common.h
	gcc -g -Wall -c -o common.o common.c
clean:
	rm server line_parser.o server.o common.o
