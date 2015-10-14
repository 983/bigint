Discontinued in favour of https://github.com/983/Num

# bigint

C usage example:
```c
#include "bigint.h"

#include <stdio.h>

int main(){
    char buf[65536];

    /* neat trick to avoid having to write &a, &b, &c everywhere */
    bigint a[1], b[1], c[1];

    /* bigints have to be initialized (same as memset'ed to zero) */
    bigint_init(a);
    bigint_init(b);
    bigint_init(c);

    /* create bigint from string */
    bigint_from_str(a, "123456789");
    bigint_from_str(b, "987654321");

    /* c = a * b */
    /* first parameter is destination parameter */
    bigint_mul(c, a, b);

    /* write and print */
    puts(bigint_write(buf, sizeof(buf), c));

    /* bigints have to be free'd */
    bigint_free(a);
    bigint_free(b);
    bigint_free(c);

    return 0;
}
```

C++ usage example:
```cpp
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
```

Implementation notes:
* BigInts consist of an array of bigint_words which are unsigned ints as defined in bigint.h
* Try adjusting bigint_word in bigint.h for maximum performance
* If there is a highest word, it should always be non-zero, as assured by bigint_raw_truncate
* Multiplication uses [the Karatsuba algorithm](https://en.wikipedia.org/wiki/Karatsuba_algorithm) for large integers
* The C++ interface is easier to use, but the C interface is better at avoiding reallocations
* If you have to calculate both division and modulo, use div_mod
* The bigint_raw_* methods expect the dst parameter to be sufficiently big, so be careful with those!

Things to do on rainy days:
* Improve performance for reading and writing bigints with 2^n bases
* Extract many digits per division for bigint_write_base
