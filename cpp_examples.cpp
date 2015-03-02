#include "num.hpp"

#include <assert.h>
#include <iostream>

int main(){
    // Nums can be created from strings or from integers
    Num a = "-1137531041259095389425522063651335971086542522289";
    Num b = "-9214001518046086468566115579527473139501";

    // Available operations:
    // +, -, *, /, %, <<, >>
    // +=, -=, *=, /=, %=, <<=, >>=
    // sqrt, gcd, div_mod, write
    Num c = a / b;
    Num d = b * c;

    // write to any output stream
    c.write(std::cout) << std::endl;
    d.write(std::cout) << std::endl;

    // available comparison operators: ==, !=, <=, >=, <, >
    assert(c == Num(123456789));
    assert(a == d);

    return 0;
}
