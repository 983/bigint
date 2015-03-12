FLAGS = -lm -O3 -Wall -Wextra -Wpedantic

all: tests c_examples cpp_examples bigint.o
	rm bigint.o

bigint.o:
	gcc -c bigint.c -o bigint.o $(FLAGS)

tests: bigint.o
	gcc tests.c bigint.o -o tests $(FLAGS)

c_examples: bigint.o
	gcc c_examples.c bigint.o -o c_examples $(FLAGS)

cpp_examples: bigint.o
	g++ cpp_examples.cpp bigint.o -o cpp_examples $(FLAGS)