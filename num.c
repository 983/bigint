#include "num.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* low bits of a * b */
num_word num_word_mul_lo(num_word a, num_word b){
    return a * b;
}

/* high bits of a * b */
num_word num_word_mul_hi(num_word a, num_word b){
    num_word c0 = NUM_WORD_LO(a) * NUM_WORD_LO(b);
    num_word c1 = NUM_WORD_LO(a) * NUM_WORD_HI(b);
    num_word c2 = NUM_WORD_HI(a) * NUM_WORD_LO(b);
    num_word c3 = NUM_WORD_HI(a) * NUM_WORD_HI(b);

    num_word c4 = NUM_WORD_HI(c0) + NUM_WORD_LO(c1) + NUM_WORD_LO(c2);
    return NUM_WORD_HI(c4) + NUM_WORD_HI(c1) + NUM_WORD_HI(c2) + c3;
}

/* dst = a + b, return carry */
num_word num_word_add_get_carry(num_word *dst, num_word a, num_word b){
    a += b;
    *dst = a;
    return a < b;
}

/* dst = a - b, return carry */
num_word num_word_sub_get_carry(num_word *dst, num_word a, num_word b){
    b = a - b;
    *dst = b;
    return b > a;
}

num_word num_word_from_char(char c){
    switch (c){
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        case 'a': case 'A': return 10;
        case 'b': case 'B': return 11;
        case 'c': case 'C': return 12;
        case 'd': case 'D': return 13;
        case 'e': case 'E': return 14;
        case 'f': case 'F': return 15;
        case 'g': case 'G': return 16;
        case 'h': case 'H': return 17;
        case 'i': case 'I': return 18;
        case 'j': case 'J': return 19;
        case 'k': case 'K': return 20;
        case 'l': case 'L': return 21;
        case 'm': case 'M': return 22;
        case 'n': case 'N': return 23;
        case 'o': case 'O': return 24;
        case 'p': case 'P': return 25;
        case 'q': case 'Q': return 26;
        case 'r': case 'R': return 27;
        case 's': case 'S': return 28;
        case 't': case 'T': return 29;
        case 'u': case 'U': return 30;
        case 'v': case 'V': return 31;
        case 'w': case 'W': return 32;
        case 'x': case 'X': return 33;
        case 'y': case 'Y': return 34;
        case 'z': case 'Z': return 35;
        default: return NUM_WORD_MAX;
    }
}

int num_word_bitlength(num_word a){
    int i;
    for (i = NUM_WORD_BITS - 1; i >= 0; i--) if ((a >> i) & 1) return i + 1;
    return 0;
}

int num_word_count_trailing_zeros(num_word a){
    int i;
    for (i = 0; i < NUM_WORD_BITS; i++) if ((a >> i) & 1) return i;
    return NUM_WORD_BITS;
}

num* num_init(num *dst){
    dst->words = NULL;
    dst->neg = dst->size = dst->capacity = 0;
    return dst;
}

num* num_reserve(num *dst, int capacity){
    if (dst->capacity >= capacity) return dst;
    dst->capacity = capacity;
    dst->words = (num_word*)realloc(dst->words, capacity * sizeof(*dst->words));
    /* out of memory? sorry :( */
    assert(dst->words != NULL);
    assert(dst->size <= capacity);
    return dst;
}

void num_free(num *dst){
    free(dst->words);
    num_init(dst);
}

int num_raw_cmp_abs(const num_word *a, int na, const num_word *b, int nb){
    int i;

    if (na > nb) return +1;
    if (na < nb) return -1;

    assert(na == nb);
    for (i = na - 1; i >= 0; i--){
        if (a[i] < b[i]) return -1;
        if (a[i] > b[i]) return +1;
    }

    return 0;
}

int num_cmp_abs(const num *a, const num *b){
    return num_raw_cmp_abs(a->words, a->size, b->words, b->size);
}

int num_raw_cmp(
    const num_word *a, int na, int a_neg,
    const num_word *b, int nb, int b_neg
){
    if (na == 0 && nb == 0) return 0;

    if (!a_neg && !b_neg) return num_raw_cmp_abs(a, na, b, nb);
    if ( a_neg &&  b_neg) return num_raw_cmp_abs(b, na, a, nb);

    return (!a_neg && b_neg) ? +1 : -1;
}

int num_cmp(const num *a, const num *b){
    return num_raw_cmp(a->words, a->size, a->neg, b->words, b->size, b->neg);
}

int num_cmp_abs_word(const num *a, num_word b){
    return num_raw_cmp_abs(a->words, a->size, &b, 1);
}

void num_raw_zero(num_word *dst, int from, int to){
    if (from >= to) return;
    memset(dst + from, 0, (to - from) * sizeof(*dst));
}

int num_raw_cpy(num_word *dst, const num_word *src, int n){
    memcpy(dst, src, n * sizeof(*src));
    return n;
}

num* num_cpy(num *dst, const num *src){
    if (src == dst) return dst;
    num_reserve(dst, src->size);
    dst->size = num_raw_cpy(dst->words, src->words, src->size);
    dst->neg = src->neg;
    assert(num_cmp(src, dst) == 0);
    return dst;
}

int num_raw_truncate(const num_word *a, int n){
    while (n > 0 && a[n - 1] == 0) n--;
    return n;
}

num* num_clr_bit(num *dst, int bit_index){
    int word_index = bit_index / NUM_WORD_BITS;
    bit_index %= NUM_WORD_BITS;

    if (word_index >= dst->size) return dst;

    dst->words[word_index] &= NUM_WORD_MAX ^ (((num_word)1) << bit_index);

    dst->size = num_raw_truncate(dst->words, dst->size);
    return dst;
}

num* num_set_bit(num *dst, int bit_index){
    int word_index = bit_index / NUM_WORD_BITS;
    int n = word_index + 1;

    num_reserve(dst, n);
    num_raw_zero(dst->words, dst->size, n);
    dst->size = NUM_MAX(dst->size, n);
    dst->words[word_index] |= ((num_word)1) << bit_index % NUM_WORD_BITS;

    return dst;
}

num_word num_get_bit(const num *src, int bit_index){
    int i = bit_index / NUM_WORD_BITS;

    if (src->size <= i) return 0;

    return (src->words[i] >> bit_index % NUM_WORD_BITS) & 1;
}

int num_raw_mul_word_add(
    num_word *dst,
    const num_word *src, int n,
    num_word factor
){
    int i;
    num_word carry = 0;

    for (i = 0; i < n; i++){
        num_word src_word = src[i];
        num_word dst_word = num_word_mul_lo(src_word, factor);
        carry  = num_word_add_get_carry(&dst_word, dst_word, carry);
        carry += num_word_mul_hi(src_word, factor);
        carry += num_word_add_get_carry(&dst[i], dst[i], dst_word);
    }

    for (; carry; i++){
        carry = num_word_add_get_carry(&dst[i], dst[i], carry);
    }

    return num_raw_truncate(dst, i);
}

int num_raw_mul_word(
    num_word *dst,
    const num_word *src, int n,
    num_word factor
){
    int i;
    num_word carry = 0;

    for (i = 0; i < n; i++){
        num_word src_word = src[i];
        num_word dst_word = num_word_mul_lo(src_word, factor);
        carry  = num_word_add_get_carry(&dst_word, dst_word, carry);
        carry += num_word_mul_hi(src_word, factor);
        dst[i] = dst_word;
    }

    if (carry){
        dst[i++] = carry;
    }

    return num_raw_truncate(dst, i);
}

int num_raw_mul_add(
    num_word *dst,
    const num_word *src_a, int na,
    const num_word *src_b, int nb
){
    int i;

    if (na == 0 || nb == 0) return 0;

    assert(dst != src_a);
    assert(dst != src_b);

    for (i = 0; i < nb; i++){
        num_raw_mul_word_add(dst + i, src_a, na, src_b[i]);
    }

    return num_raw_truncate(dst, na + nb);
}

int num_raw_add_word(
    num_word *dst,
    const num_word *src, int n,
    num_word b
){
    int i;
    num_word carry = b;

    for (i = 0; i < n; i++){
        carry = num_word_add_get_carry(&dst[i], src[i], carry);
    }

    for (; carry; i++){
        carry = num_word_add_get_carry(&dst[i], dst[i], carry);
    }

    return num_raw_truncate(dst, i);
}

int num_raw_from_str_base(num_word *dst, const char *src, int base){
    int n = 0;

    for (; *src; src++){
        num_word digit = num_word_from_char(*src);

        if (digit == NUM_WORD_MAX) continue;

        n = num_raw_mul_word(dst, dst, n, base);
        n = num_raw_add_word(dst, dst, n, digit);
    }

    return num_raw_truncate(dst, n);
}

int num_count_digits(const char *src){
    int n = 0;
    for (; *src; src++) if (num_word_from_char(*src) != NUM_WORD_MAX) n++;
    return n;
}

int num_raw_add(
    num_word *dst,
    const num_word *src_a, int na,
    const num_word *src_b, int nb
){
    num_word sum, carry = 0;
    int i, n = NUM_MIN(na, nb);

    for (i = 0; i < n; i++){
        carry  = num_word_add_get_carry(&sum, carry, src_a[i]);
        carry += num_word_add_get_carry(&sum, sum  , src_b[i]);
        dst[i] = sum;
    }

    for (; i < na; i++){
        carry = num_word_add_get_carry(&dst[i], src_a[i], carry);
    }

    for (; i < nb; i++){
        carry = num_word_add_get_carry(&dst[i], src_b[i], carry);
    }

    if (carry) dst[i++] = carry;

    return num_raw_truncate(dst, i);
}

int num_raw_sub(
    num_word *dst,
    const num_word *src_a, int na,
    const num_word *src_b, int nb
){
    num_word dif, carry = 0;
    int i;
    assert(na >= nb);
    assert(num_raw_cmp_abs(src_a, na, src_b, nb) >= 0);

    for (i = 0; i < nb; i++){
        carry  = num_word_sub_get_carry(&dif, src_a[i], carry);
        carry += num_word_sub_get_carry(&dif, dif, src_b[i]);
        dst[i] = dif;
    }

    for (; i < na; i++){
        carry = num_word_sub_get_carry(&dst[i], src_a[i], carry);
    }

    assert(!carry);
    return num_raw_truncate(dst, i);
}

int num_raw_mul_karatsuba(
    num_word *dst,
    const num_word *a, int na,
    const num_word *b, int nb,
    num_word *tmp
){
    /* so many */
    int n, k, m, m2;
    const num_word *lo1, *hi1, *lo2, *hi2;
    int nlo1, nhi1, nlo2, nhi2;
    num_word *lo1hi1, *lo2hi2, *z0, *z1, *z2;
    int nlo1hi1, nlo2hi2, nz0, nz1, nz2;

    if (na < NUM_KARATSUBA_WORD_THRESHOLD && nb < NUM_KARATSUBA_WORD_THRESHOLD){
        num_raw_zero(dst, 0, na + nb);
        return num_raw_mul_add(dst, a, na, b, nb);
    }

    m = NUM_MAX(na, nb);
    m2 = m / 2;
    k = m2 + 2;

    lo1 = a;
    lo2 = b;
    hi1 = a + m2;
    hi2 = b + m2;

    nlo1 = num_raw_truncate(lo1, NUM_MIN(m2, na));
    nlo2 = num_raw_truncate(lo2, NUM_MIN(m2, nb));
    nhi1 = num_raw_truncate(hi1, NUM_MAX(na - m2, 0));
    nhi2 = num_raw_truncate(hi2, NUM_MAX(nb - m2, 0));

    lo1hi1 = tmp; tmp += k;
    lo2hi2 = tmp; tmp += k;
    z0 = tmp; tmp += k*2;
    z1 = tmp; tmp += k*2;
    z2 = tmp; tmp += k*2;

    nlo1hi1 = num_raw_add(lo1hi1, lo1, nlo1, hi1, nhi1);
    nlo2hi2 = num_raw_add(lo2hi2, lo2, nlo2, hi2, nhi2);

    nz0 = num_raw_mul_karatsuba(z0, lo1   , nlo1   , lo2   , nlo2   , tmp);
    nz1 = num_raw_mul_karatsuba(z1, lo1hi1, nlo1hi1, lo2hi2, nlo2hi2, tmp);
    nz2 = num_raw_mul_karatsuba(z2,    hi1, nhi1   ,    hi2,    nhi2, tmp);

    nz1 = num_raw_sub(z1, z1, nz1, z0, nz0);
    nz1 = num_raw_sub(z1, z1, nz1, z2, nz2);

    n = nz0;

    num_raw_cpy(dst, z0, n);
    num_raw_zero(dst, n, na + nb);

    n = num_raw_add(dst + m2*1, dst + m2*1, NUM_MAX(n - m2, 0), z1, nz1);
    n = num_raw_add(dst + m2*2, dst + m2*2, NUM_MAX(n - m2, 0), z2, nz2);

    return num_raw_truncate(dst, n + m2*2);
}

num* num_mul(num *dst, const num *a, const num *b){
    int na = a->size;
    int nb = b->size;
    int n = na + nb;
    num_word *tmp;

    num_reserve(dst, n);

    /* bound found through experimentation */
    tmp = (num_word*)malloc(sizeof(*tmp)*(NUM_MAX(na, nb) * 11 + 180 + n));

    dst->size = num_raw_mul_karatsuba(tmp, a->words, na, b->words, nb, tmp + n);
    num_raw_cpy(dst->words, tmp, dst->size);
    dst->neg = a->neg ^ b->neg;

    free(tmp);
    return dst;
}

int num_digits_bound(int n_digits_src, double src_base, double dst_base){
    /* +1 for rounding errors, just in case */
    return ceil(n_digits_src * log(src_base) / log(dst_base)) + 1;
}

int num_write_size(const num *a, double dst_base){
    double src_base = pow(2, NUM_WORD_BITS);
    return num_digits_bound(a->size, src_base, dst_base)
        + sizeof('-') + sizeof('\0');
}

num* num_from_str_base(num *dst, const char *src, int src_base){
    int n_digits_src, n_digits_dst;
    /* yes, this will fit, up to 2^10 bit num_words */
    double dst_base = pow(2.0, NUM_WORD_BITS);

    n_digits_src = num_count_digits(src);
    n_digits_dst = num_digits_bound(n_digits_src, src_base, dst_base);

    num_reserve(dst, n_digits_dst);
    dst->size = n_digits_dst;
    num_raw_zero(dst->words, 0, n_digits_dst);

    dst->size = num_raw_from_str_base(dst->words, src, src_base);
    dst->neg = *src == '-';
    return dst;
}

num* num_from_int(num *dst, int src){
    /* be careful with -INT_MIN which does not fit into int */
    unsigned int x = src >= 0 ? src : -src;
    int n = NUM_MAX(1, sizeof(x)/sizeof(num_word));
    num_reserve(dst, n);
    num_raw_zero(dst->words, 0, n);
    memcpy(dst->words, &x, sizeof(x));
    dst->neg = src < 0;
    dst->size = num_raw_truncate(dst->words, n);
    return dst;
}

num* num_from_word(num *dst, num_word a){
    num_reserve(dst, 1);
    dst->neg = 0;
    dst->words[0] = a;
    dst->size = num_raw_truncate(dst->words, 1);
    return dst;
}

int num_raw_add_signed(
    num_word *dst, int *dst_neg,
    const num_word *a, int na, int a_neg,
    const num_word *b, int nb, int b_neg
){
    if (a_neg){
        if (b_neg){
            if (na >= nb){
                *dst_neg = 1;
                return num_raw_add(dst, a, na, b, nb);
            }else{
                *dst_neg = 1;
                return num_raw_add(dst, b, nb, a, na);
            }
        }else{
            if (num_raw_cmp_abs(a, na, b, nb) >= 0){
                *dst_neg = 1;
                return num_raw_sub(dst, a, na, b, nb);
            }else{
                *dst_neg = 0;
                return num_raw_sub(dst, b, nb, a, na);
            }
        }
    }else{
        if (b_neg){
            if (num_raw_cmp_abs(a, na, b, nb) >= 0){
                *dst_neg = 0;
                return num_raw_sub(dst, a, na, b, nb);
            }else{
                *dst_neg = 1;
                return num_raw_sub(dst, b, nb, a, na);
            }
        }else{
            if (na >= nb){
                *dst_neg = 0;
                return num_raw_add(dst, a, na, b, nb);
            }else{
                *dst_neg = 0;
                return num_raw_add(dst, b, nb, a, na);
            }
        }
    }
}

num* num_add_signed(num *dst, const num *a, int a_neg, const num *b, int b_neg){
    int na = a->size;
    int nb = b->size;
    int n = NUM_MAX(na, nb) + 1;

    num_reserve(dst, n);

    dst->size = num_raw_add_signed(
        dst->words, &dst->neg,
        a->words, na, a_neg,
        b->words, nb, b_neg
    );

    return dst;
}

num* num_add(num *dst, const num *a, const num *b){
    return num_add_signed(dst, a, a->neg, b, b->neg);
}

num* num_sub(num *dst, const num *a, const num *b){
    return num_add_signed(dst, a, a->neg, b, !b->neg);
}

num* num_add_word_signed(num *dst, const num *src_a, num_word b, int b_neg){
    int na = src_a->size;

    num_reserve(dst, na + 1);

    dst->size = num_raw_add_signed(
        dst->words, &dst->neg,
        src_a->words, na, src_a->neg,
        &b, 1, b_neg
    );

    return dst;
}

num* num_add_word(num *dst, const num *src_a, num_word b){
    return num_add_word_signed(dst, src_a, b, 0);
}

num* num_sub_word(num *dst, const num *src_a, num_word b){
    return num_add_word_signed(dst, src_a, b, 1);
}

int num_raw_shift_left(
    num_word *dst,
    const num_word *src, int n_src,
    int shift
){
    int i;
    int word_shift = shift / NUM_WORD_BITS;
    int bits_shift = shift % NUM_WORD_BITS;

    if (bits_shift){
        num_word lo, hi = 0;

        for (i = n_src + word_shift; i > word_shift; i--){
            lo = src[i - word_shift - 1];
            dst[i] = (hi << bits_shift) | (lo >> (NUM_WORD_BITS - bits_shift));
            hi = lo;
        }

        for (i = word_shift; i >= 0; i--){
            lo = 0;
            dst[i] = (hi << bits_shift) | (lo >> (NUM_WORD_BITS - bits_shift));
            hi = lo;
        }

        return num_raw_truncate(dst, n_src + word_shift + 1);
    }else{
        /* this case is not only separate because of performance */
        /* but (lo >> (NUM_WORD_BITS - 0)) is also undefined behaviour */
        for (i = n_src + word_shift - 1; i >= word_shift; i--){
            dst[i] = src[i - word_shift];
        }

        for (i = word_shift - 1; i >= 0; i--){
            dst[i] = 0;
        }

        return num_raw_truncate(dst, n_src + word_shift);
    }
}

int num_raw_shift_right(
    num_word *dst,
    const num_word *src, int n_src,
    int shift
){
    int i;
    int word_shift = shift / NUM_WORD_BITS;
    int bits_shift = shift % NUM_WORD_BITS;

    if (bits_shift){
        num_word hi, lo = src[word_shift];

        for (i = 0; i < n_src - word_shift - 1; i++){
            hi = src[i + word_shift + 1];
            dst[i] = (hi << (NUM_WORD_BITS - bits_shift)) | (lo >> bits_shift);
            lo = hi;
        }

        for (i = NUM_MAX(n_src - word_shift - 1, 0); i < n_src; i++){
            hi = 0;
            dst[i] = (hi << (NUM_WORD_BITS - bits_shift)) | (lo >> bits_shift);
            lo = hi;
        }

        return num_raw_truncate(dst, n_src);
    }else{
        /* this case is not only separate because of performance */
        /* but (hi << (NUM_WORD_BITS - 0)) is also undefined behaviour */
        for (i = 0; i < n_src - word_shift; i++){
            dst[i] = src[i + word_shift];
        }

        for (i = NUM_MAX(n_src - word_shift, 0); i < n_src; i++){
            dst[i] = 0;
        }

        return num_raw_truncate(dst, NUM_MAX(n_src - word_shift, 0));
    }
}

num* num_shift_left(num *dst, const num *src, int shift){
    int n = src->size + shift / NUM_WORD_BITS + (shift % NUM_WORD_BITS != 0);
    num_reserve(dst, n);
    dst->size = num_raw_shift_left(dst->words, src->words, src->size, shift);
    dst->neg = src->neg;
    return dst;
}

num* num_shift_right(num *dst, const num *src, int shift){
    num_reserve(dst, src->size);
    dst->size = num_raw_shift_right(dst->words, src->words, src->size, shift);
    dst->neg = src->neg;
    return dst;
}

int num_bitlength(const num *a){
    int last = a->size - 1;
    if (last < 0) return 0;
    return num_word_bitlength(a->words[last]) + last*NUM_WORD_BITS;
}

int num_count_trailing_zeros(const num *a){
    int i;
    for (i = 0; i < a->size; i++){
        num_word w = a->words[i];
        if (w) return num_word_count_trailing_zeros(w) + i*NUM_WORD_BITS;
    }
    return a->size * NUM_WORD_BITS;
}

num* num_div_mod(
    num *dst_quotient,
    num *dst_remainder,
    const num *src_numerator,
    const num *src_denominator
){
    int shift;
    num denominator[1], *remainder = dst_remainder, *quotient = dst_quotient;

    if (src_denominator->size == 0) return NULL;

    /* fast path for native word size */
    if (src_numerator->size == 1 && src_denominator->size == 1){
        num_word a = src_numerator->words[0];
        num_word b = src_denominator->words[0];
        num_from_word(quotient, a / b);
        num_from_word(remainder, a % b);
        quotient->neg = src_numerator->neg ^ src_denominator->neg;
        remainder->neg = src_numerator->neg;
        return dst_quotient;
    }

    /* fast path for half word size */
    if (src_denominator->size == 1 &&
        src_denominator->words[0] <= NUM_HALF_WORD_MAX
    ){
        num_word rem;
        num_cpy(quotient, src_numerator);
        num_div_mod_half_word(quotient, &rem, src_denominator->words[0]);
        num_from_word(remainder, rem);
        quotient->neg = src_numerator->neg ^ src_denominator->neg;
        remainder->neg = src_numerator->neg;
        return dst_quotient;
    }

    num_cpy(remainder, src_numerator);
    remainder->neg = 0;
    quotient->size = 0;

    if (num_cmp_abs(remainder, src_denominator) >= 0){
        shift = num_bitlength(remainder) - num_bitlength(src_denominator);

        num_init(denominator);
        num_shift_left(denominator, src_denominator, shift);
        denominator->neg = 0;

        /* divide bit by bit */
        for (; shift >= 0; shift--) {
            if (num_cmp_abs(remainder, denominator) >= 0) {
                num_sub(remainder, remainder, denominator);
                num_set_bit(quotient, shift);
            }
            num_shift_right(denominator, denominator, 1);
        }

        num_free(denominator);
    }

    quotient->neg = src_numerator->neg ^ src_denominator->neg;
    remainder->neg = src_numerator->neg;
    return dst_quotient;
}

num* num_div_mod_half_word(
    num *dst,
    num_word *dst_remainder,
    num_word denominator
){
    int i, j;
    num_word parts[2], div_word, mod_word, remainder = 0;

    assert(denominator != 0);
    assert(denominator <= NUM_HALF_WORD_MAX);

    for (i = dst->size - 1; i >= 0; i--){
        num_word dst_word = 0;
        num_word src_word = dst->words[i];
        parts[1] = NUM_WORD_LO(src_word);
        parts[0] = NUM_WORD_HI(src_word);

        /* divide by denominator twice, keeping remainder in mind */
        for (j = 0; j < 2; j++){
            remainder <<= NUM_WORD_BITS / 2;
            remainder |= parts[j];

            div_word = remainder / denominator;
            mod_word = remainder % denominator;
            remainder = mod_word;

            dst_word <<= NUM_WORD_BITS / 2;
            dst_word |= div_word;
        }

        dst->words[i] = dst_word;
    }

    *dst_remainder = remainder;
    dst->size = num_raw_truncate(dst->words, dst->size);
    return dst;
}

num* num_gcd(num *dst, const num *src_a, const num *src_b){
    int shift, shift_a, shift_b;
    num a[1], b[1];

    if (src_a->size == 0){
        num_cpy(dst, src_b);
        dst->neg = 0;
        return dst;
    }

    if (src_b->size == 0){
        num_cpy(dst, src_a);
        dst->neg = 0;
        return dst;
    }

    num_init(a);
    num_init(b);

    shift_a = num_count_trailing_zeros(src_a);
    shift_b = num_count_trailing_zeros(src_b);
    shift = NUM_MIN(shift_a, shift_b);

    num_shift_right(a, src_a, shift_a);
    num_shift_right(b, src_b, shift);
    a->neg = 0;
    b->neg = 0;

    do {
        num_shift_right(b, b, num_count_trailing_zeros(b));

        if (num_cmp_abs(a, b) > 0) NUM_SWAP(num, *a, *b);

        num_sub(b, b, a);
    } while (b->size != 0);

    num_shift_left(dst, a, shift);

    num_free(a);
    num_free(b);
    return dst;
}

num* num_sqrt(num *dst, const num *src){
    int bit;
    num sum[1], tmp[1];
    const double MAX_INT_THAT_FITS_IN_DOUBLE = pow(2.0, 52.0);

    dst->neg = 0;
    dst->size = 0;

    if (src->size == 0) return dst;

    if (src->size == 1 && src->words[0] < MAX_INT_THAT_FITS_IN_DOUBLE){
        num_from_word(dst, sqrt(src->words[0]));
        return dst;
    }

    num_init(sum);
    num_init(tmp);

    num_cpy(tmp, src);
    tmp->neg = 0;

    /* index of highest 1 bit rounded down */
    bit = num_bitlength(src);
    if (bit & 1) bit ^= 1;

    for (; bit >= 0; bit -= 2){
        num_cpy(sum, dst);
        num_set_bit(sum, bit);

        if (num_cmp_abs(tmp, sum) >= 0){
            num_sub(tmp, tmp, sum);
            num_set_bit(dst, bit + 1);
        }

        num_shift_right(dst, dst, 1);
    }

    num_free(tmp);
    num_free(sum);
    return dst;
}

char* num_write_base(char *dst, int n, const num *a, num_word base){
    int i = 0;
    static const char *table = "0123456789abcdefghijklmnopqrstuvwxyz";
    assert(base >= 2 && base <= 36);

    if (i < n) dst[i++] = '\0';

    if (a->size == 0){
        if (i < n) dst[i++] = '0';
    }else{
        num tmp[1];
        num_init(tmp);
        num_cpy(tmp, a);

        while (tmp->size > 0){
            num_word remainder;
            /* TODO extract as many digits as fit into num_word at once */
            /* tricky with leading zeros */
            num_div_mod_half_word(tmp, &remainder, base);
            if (i < n) dst[i++] = table[remainder];
        }

        num_free(tmp);
    }

    if (a->neg) if (i < n) dst[i++] = '-';
    NUM_REVERSE(char, dst, i);

    return dst;
}

num* num_rand_bits(num *dst, int n_bits, num_rand_func rand_func){
    int n_word_bits = n_bits % NUM_WORD_BITS;
    int n_words = n_bits / NUM_WORD_BITS + (n_word_bits != 0);

    num_reserve(dst, n_words);

    rand_func((uint8_t*)dst->words, sizeof(*dst->words) * n_words);

    if (n_word_bits){
        dst->words[n_words - 1] >>= NUM_WORD_BITS - n_word_bits;
    }

    dst->size = num_raw_truncate(dst->words, n_words);
    return dst;
}

num* num_rand_inclusive(num *dst, const num *n, num_rand_func rand_func){
    int n_bits = num_bitlength(n);

    do {
        num_rand_bits(dst, n_bits, rand_func);
    } while (num_cmp(dst, n) > 0);

    return dst;
}

num* num_rand_exclusive(num *dst, const num *n, num_rand_func rand_func){
    int n_bits = num_bitlength(n);

    do {
        num_rand_bits(dst, n_bits, rand_func);
    } while (num_cmp(dst, n) >= 0);

    return dst;
}

num* num_pow_mod(
    num *dst,
    const num *src_base,
    const num *src_exponent,
    const num *src_modulus
){
    num base[1], exponent[1], tmp[1], unused[1], modulus[1];

    num_init(base);
    num_init(exponent);
    num_init(tmp);
    num_init(unused);
    num_init(modulus);

    num_cpy(exponent, src_exponent);
    num_cpy(modulus, src_modulus);
    num_div_mod(unused, base, src_base, modulus);
    num_from_word(dst, 1);

    for (; exponent->size; num_shift_right(exponent, exponent, 1)){
        if (num_get_bit(exponent, 0)){
            num_mul(tmp, dst, base);
            num_div_mod(unused, dst, tmp, modulus);
        }
        num_mul(tmp, base, base);
        num_div_mod(unused, base, tmp, modulus);
    }

    num_free(base);
    num_free(exponent);
    num_free(tmp);
    num_free(unused);
    num_free(modulus);
    return dst;
}

int num_is_probable_prime(const num *n, int n_tests, num_rand_func rand_func){
    num a[1], d[1], x[1], two[1], n_minus_one[1], n_minus_three[1];
    int i, shift;

    /* divisible by 2, not prime */
    if (num_get_bit(n, 0) == 0) return 0;

    /* 1, 3 are prime */
    if (num_cmp_abs_word(n, 3) <= 0) return 1;

    num_init(a);
    num_init(d);
    num_init(x);
    num_init(two);
    num_init(n_minus_one);
    num_init(n_minus_three);

    num_from_word(two, 2);

    num_sub_word(n_minus_one, n, 1);
    num_sub_word(n_minus_three, n, 3);

    shift = num_count_trailing_zeros(n_minus_one);
    num_shift_right(d, n_minus_one, shift);

    do {
        num_rand_inclusive(a, n_minus_three, rand_func);
        num_add_word(a, a, 2);
        num_pow_mod(x, a, d, n);

        if (num_cmp_abs_word(x, 1) == 0) continue;
        if (num_cmp(x, n_minus_one) == 0) continue;

        for (i = 1; i < shift; i++){
            num_pow_mod(x, x, two, n);
            if (num_cmp_abs_word(x, 1) == 0) return 0;
            if (num_cmp(x, n_minus_one) == 0) break;
        }

        if (i == shift) return 0;
    } while (--n_tests);

    num_free(a);
    num_free(d);
    num_free(x);
    num_free(two);
    num_free(n_minus_one);
    num_free(n_minus_three);
    return 1;
}

num* num_pow_word(num *dst, const num *base, num_word exponent){
    num result[1], p[1];

    num_init(p);
    num_init(result);

    num_cpy(p, base);
    num_from_word(result, 1);

    for (; exponent; exponent >>= 1){
        if (exponent & 1){
            num_mul(result, result, p);
            exponent--;
        }
        num_mul(p, p, p);
    }

    num_cpy(dst, result);
    num_free(p);
    num_free(result);
    return dst;
}
