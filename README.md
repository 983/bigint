# num

C usage example:
```c
#include "num.h"

#include <stdio.h>

int main(){
    char buf[65536];

    /* neat trick to avoid having to write &a, &b, &c everywhere */
    num a[1], b[1], c[1];

    /* nums have to be inited (or memset'ed to 0) */
    num_init(a);
    num_init(b);
    num_init(c);

    /* create num from string, bases 2 to 36 are allowed */
    num_from_str_base(a, "123456789", 10);
    num_from_str_base(b, "987654321", 10);

    /* c = a * b */
    /* first parameter is destination parameter */
    num_mul(c, a, b);

    /* bases 2 to 36 are allowed */
    num_write_base(buf, sizeof(buf), c, 10);

    puts(buf);

    /* nums have to be free'd */
    num_free(a);
    num_free(b);
    num_free(c);

    /* See tests.c for all possible operations */

    return 0;
}
```

C++ usage example:
```cpp
#include "num.hpp"

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
    // Nums can be created from strings or from integers
    Num a = "-1137531041259095389425522063651335971086542522289";
    Num b = "-9214001518046086468566115579527473139501";

    // Available operators:
    // +, -, *, /, %, <<, >>
    // +=, -=, *=, /=, %=, <<=, >>=, ++, --
    // ==, !=, <=, >=, <, >
    Num c = a / b;
    Num d = b * c;
    assert(c == 123456789);
    assert(a == d);

    // write to any output stream
    c.write(std::cout) << std::endl;
    d.write(std::cout) << std::endl;

    // find the biggest probable prime less than 10^50
    Num p = Num(10).pow(50) - 1;

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
* Numbers consist of an array of num_words which are unsigned ints as defined in num.h
* Try adjusting num_word in num.h for maximum performance
* If there is a highest word, it should always be non-zero, as assured by num_raw_truncate
* Multiplication uses [the Karatsuba algorithm](https://en.wikipedia.org/wiki/Karatsuba_algorithm) for large integers
* The C++ interface is easier to use, but the C interface is better at avoiding reallocations
* If you have to calculate both division and modulo, use div_mod
* The num_raw_* methods expect the dst parameter to be sufficiently big, so be careful with those!

Things to do on rainy days:
* Improve performance for reading and writing nums with 2^n bases
* Extract many digits per division for num_write_base