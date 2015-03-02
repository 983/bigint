FLAGS = -lm -O3 -Wall -Wextra -Wpedantic

all: tests c_examples cpp_examples num.o
	rm num.o

num.o:
	gcc -c num.c -o num.o $(FLAGS)

tests: num.o
	gcc tests.c num.o -o tests $(FLAGS)

c_examples: num.o
	gcc c_examples.c num.o -o c_examples $(FLAGS)

cpp_examples: num.o
	g++ cpp_examples.cpp num.o -o cpp_examples $(FLAGS)