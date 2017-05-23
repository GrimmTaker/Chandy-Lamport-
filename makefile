#Makefile
CC=g++
CFLAGS=-I.
all: battle


battle: general.o process_func.o caeser.o
	g++ general.o process_func.o  caeser.o -o battle 


general.o:general.c++ process_func.c++ caeser.c++
	g++ -c general.c++ process_func.c++ caeser.c++ -g

clean:
	rm -r 	*.o 


