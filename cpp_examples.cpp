#include "bigint.hpp"

#include <assert.h>
#include <iostream>

uint32_t xorshift32() {
    static uint32_t x = 314159265;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return x;
}

// do not use this as a cryptographically secure random number generator
void not_secure_random(uint8_t *dst, int n){
    for (int i = 0; i < n; i++) dst[i] = xorshift32();
}

int main(){
    // BigInts can be created from strings or from integers
    BigInt a = "-1137531041259095389425522063651335971086542522289";
    BigInt b = "-9214001518046086468566115579527473139501";

    // Available operators:
    // +, -, *, /, %, <<, >>
    // +=, -=, *=, /=, %=, <<=, >>=, ++, --
    // ==, !=, <=, >=, <, >
    BigInt c = a / b;
    BigInt d = b * c;
    assert(c == 123456789);
    assert(a == d);

    // write to any output stream
    c.write(std::cout) << std::endl;
    d.write(std::cout) << std::endl;

    // find the biggest probable prime less than 10^42
    BigInt p = BigInt(10).pow(42) - 1;

    for (int i = 0; i < 100; i++){
        if (p.is_probable_prime(10, not_secure_random)){
            p.write(std::cout << "Big prime: ") << std::endl;
            break;
        }
        --p;
    }

    return 0;
}
