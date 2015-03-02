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
