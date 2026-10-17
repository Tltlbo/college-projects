main : main.o signature.o
	gcc -g -o main main.o signature.o
main.o : main.c signature.h
	gcc -g -c main.c
signature.o : signature.c
	gcc -g -c signature.c
