#include "bigint.h"

#include <stdio.h>

int main(){
    char buf[65536];

    /* neat trick to avoid having to write &a, &b, &c everywhere */
    bigint a[1], b[1], c[1];

    /* bigints have to be inited (or memset'ed to 0) */
    bigint_init(a);
    bigint_init(b);
    bigint_init(c);

    /* create bigint from string, bases 2 to 36 are allowed */
    bigint_from_str_base(a, "123456789", 10);
    bigint_from_str_base(b, "987654321", 10);

    /* c = a * b */
    /* first parameter is destination parameter */
    bigint_mul(c, a, b);

    /* bases 2 to 36 are allowed */
    bigint_write_base(buf, sizeof(buf), c, 10);

    puts(buf);

    /* bigints have to be free'd */
    bigint_free(a);
    bigint_free(b);
    bigint_free(c);

    /* See tests.c for all possible operations */

    return 0;
}
