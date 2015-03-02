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
```

Implementation notes:
* Numbers consist of an array of num_words as defined in num.h
* Try adjusting num_word in num.h to your native word size for maximum performance
* If there is a highest word, it should always be non-zero, as assured by num_raw_truncate
* Multiplication uses [the Karatsuba algorithm](https://en.wikipedia.org/wiki/Karatsuba_algorithm) for large integers
* The C++ interface is easier to use, but the C interface is better at avoiding reallocations
* Division and modulo of numbers bigger than NUM_WORD_MAX is slow
* If you have to calculate both division and modulo, use div_mod
* If your denominator fits into half a word and your nominator does not, try num_div_mod_half_word
* The num_raw_* methods expect the dst parameter to be sufficiently big, so be careful with those!

Things to do on rainy days:
* Improve performance for reading and writing nums with 2^n bases
* Extract many digits per division for num_write_base