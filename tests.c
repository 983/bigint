#include "num.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int gcd(int a, int b){
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    while (a){
        int tmp = a;
        a = b % a;
        b = tmp;
    }
    return b;
}

int main(){
    int i, j;
    char buf[65536];
    num a[1], b[1], c[1], d[1], e[20];
    const char *text = "123456790123456790120987654320987654321";

    num_init(a);
    num_init(b);
    num_init(c);
    num_init(d);

    for (i = 0; i < 20; i++) num_init(e + i);

    num_from_str_base(a, text, 10);
    num_from_str_base(b, "11111111111111111111", 10);
    num_sqrt(c, a);
    assert(num_cmp(b, c) == 0);

    num_from_str_base(a, "1", 10);
    num_from_str_base(b, "1", 10);
    num_shift_left(a, a, 1 << 10);

    num_mul(c, a, a);
    num_sub(c, c, a);
    num_sub(c, c, a);
    num_add(c, c, b);

    num_sub(a, a, b);
    num_mul(d, a, a);

    assert(num_cmp(c, d) == 0);

    num_from_str_base(a, text, 10);

    i = strcmp(text, num_write_base(buf, sizeof(buf), a, 10));
    assert(i == 0);

    for (i = 0; i < 12345; i++){
        int x = rand() % 12345;
        int y = rand() % 12345;
        int shift = rand() % 1234;
        if (rand() & 1) x = -x;
        if (rand() & 1) y = -y;

        num_from_int(a, x);
        num_from_int(b, y);
        num_from_int(e + 0, x + y);
        num_from_int(e + 1, x - y);
        num_from_int(e + 2, x * y);

        if (y != 0){
            num_from_int(e + 3, x / y);
            num_from_int(e + 4, x % y);
        }

        num_from_int(e + 5, sqrt(x > 0 ? x : -x));
        num_from_int(e + 6, gcd(x, y));

        num_cpy(c, a);
        num_shift_left(a, a, shift);
        num_shift_right(a, a, shift);

        assert(num_cmp(a, c) == 0);

        num_add(e + 10, a, b);
        num_sub(e + 11, a, b);
        num_mul(e + 12, a, b);
        num_div_mod(e + 13, e + 14, a, b);
        num_sqrt(e + 15, a);
        num_gcd(e + 16, a, b);

        for (j = 0; j < 7; j++){
            if (y == 0 && (j == 3 || j == 4)) continue;
            if (num_cmp(e + j, e + j + 10) != 0){
                printf("i %i, j %i failed for numbers %i, %i\n", i, j, x, y);
            }
            assert(num_cmp(e + j, e + j + 10) == 0);
        }
    }

    num_free(a);
    num_free(b);
    num_free(c);
    num_free(d);

    for (i = 0; i < 20; i++) num_free(e + i);

    puts("tests passed");

    return 0;
}

