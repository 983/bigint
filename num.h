#ifndef NUM_H_INCLUDED
#define NUM_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <limits.h>
#include <stdint.h>

/* any unsigned integer type */
typedef uint32_t num_word;

#define NUM_KARATSUBA_WORD_THRESHOLD 20

#define NUM_WORD_BITS ((int)(sizeof(num_word) * CHAR_BIT))
#define NUM_WORD_MAX ((num_word)-1)
#define NUM_HALF_WORD_MAX (NUM_WORD_MAX >> NUM_WORD_BITS / 2)

#define NUM_WORD_LO(a) ((a) & NUM_HALF_WORD_MAX)
#define NUM_WORD_HI(a) ((a) >> sizeof(a) * CHAR_BIT / 2)

#define NUM_MIN(a, b) ((a) < (b) ? (a) : (b))
#define NUM_MAX(a, b) ((a) > (b) ? (a) : (b))

#define NUM_SWAP(type, a, b) do { type _tmp = a; a = b; b = _tmp; } while (0)

#define NUM_REVERSE(type, data, n) do {\
    int _i;\
    for (_i = 0; _i < (n)/2; _i++) NUM_SWAP(type, data[_i], data[n - 1 - _i]);\
} while (0)

typedef struct num {
    num_word *words;
    int neg, size, capacity;
} num;

typedef void (*num_rand_func)(uint8_t *dst, int n);

num_word num_word_mul_lo(num_word a, num_word b);
num_word num_word_mul_hi(num_word a, num_word b);

num_word num_word_add_get_carry(num_word *dst, num_word a, num_word b);
num_word num_word_sub_get_carry(num_word *dst, num_word a, num_word b);

num_word num_word_from_char(char c);

int num_word_bitlength(num_word a);
int num_word_count_trailing_zeros(num_word a);

num* num_init(num *a);
num* num_reserve(num *a, int capacity);
void num_free(num *a);

int num_cmp_abs(const num *a, const num *b);
int num_cmp(const num *a, const num *b);
int num_cmp_abs_word(const num *a, num_word b);

num* num_cpy(num *dst, const num *src);

num*     num_clr_bit(num *dst, int bit_index);
num*     num_set_bit(num *dst, int bit_index);
num_word num_get_bit(const num *src, int bit_index);
num* num_mul(num *dst, const num *a, const num *b);

int num_count_digits(const char *src);
int num_digits_bound(int n_digits_src, double src_base, double dst_base);
int num_write_size(const num *a, double dst_base);
num* num_from_str_base(num *dst, const char *src, int src_base);
num* num_from_int(num *dst, int src);
num* num_from_word(num *dst, num_word a);

num* num_add_signed(num *dst, const num *a, int a_neg, const num *b, int b_neg);
num* num_add(num *dst, const num *a, const num *b);
num* num_sub(num *dst, const num *a, const num *b);
num* num_add_word_signed(num *dst, const num *src_a, num_word b, int b_neg);
num* num_add_word(num *dst, const num *src_a, num_word b);
num* num_sub_word(num *dst, const num *src_a, num_word b);

char* num_write_base(char *dst, int n, const num *a, num_word base);

num* num_shift_left(num *dst, const num *src, int shift);
num* num_shift_right(num *dst, const num *src, int shift);

int num_bitlength(const num *a);
int num_count_trailing_zeros(const num *a);

num* num_div_mod(
    num *dst_quotient,
    num *dst_remainder,
    const num *src_numerator,
    const num *src_denominator
);

num* num_div_mod_half_word(
    num *dst,
    num_word *dst_remainder,
    num_word denominator
);

num* num_gcd(num *dst, const num *src_a, const num *src_b);
num* num_sqrt(num *dst, const num *src);

num* num_rand_bits(num *dst, int n_bits, num_rand_func rand_func);
num* num_rand_inclusive(num *dst, const num *n, num_rand_func rand_func);
num* num_rand_exclusive(num *dst, const num *n, num_rand_func rand_func);

num* num_pow_mod(
    num *dst,
    const num *src_base,
    const num *src_exponent,
    const num *src_modulus
);

/* probability for wrong positives is approximately 1/4^n_tests */
int num_is_probable_prime(const num *n, int n_tests, num_rand_func rand_func);

num* num_pow_word(num *dst, const num *src, num_word exponent);

#ifdef __cplusplus
}
#endif

#endif
