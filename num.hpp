#ifndef NUM_HPP_INCLUDED
#define NUM_HPP_INCLUDED

#include "num.h"

#include <stdlib.h>

struct Num {
    num data[1];

    Num(){
        num_init(data);
    }

    Num(int b){
        num_init(data);
        num_from_int(data, b);
    }

    Num(const char *s, int base = 10){
        num_init(data);
        num_from_str_base(data, s, base);
    }

    Num(const Num &b){
        num_init(data);
        num_cpy(data, b.data);
    }

    Num& operator = (const Num &b){
        num_cpy(data, b.data);
        return *this;
    }

    ~Num(){
        num_free(data);
    }

    static void div_mod(
        Num &quotient, Num &remainder,
        const Num &numerator,
        const Num &denominator
    ){
        num_div_mod(quotient.data, remainder.data,
            numerator.data, denominator.data);
    }

    void write(char *buf, int n, int dst_base = 10) const {
        num_write_base(buf, n, data, dst_base);
    }

    template <class STREAM>
    STREAM& write(STREAM &s, int dst_base = 10) const {
        int n = num_write_size(data, dst_base);
        char *buf = (char*)malloc(n);
        write(buf, n, dst_base);
        s << buf;
        free(buf);
        return s;
    }

    Num& operator <<= (int shift){
        num_shift_left(data, data, shift);
        return *this;
    }

    Num& operator >>= (int shift){
        num_shift_right(data, data, shift);
        return *this;
    }

    Num& operator += (const Num &b){
        num_add(data, data, b.data);
        return *this;
    }

    Num& operator -= (const Num &b){
        num_sub(data, data, b.data);
        return *this;
    }

    Num& operator *= (const Num &b){
        num_mul(data, data, b.data);
        return *this;
    }

    Num& operator /= (const Num &b){
        Num quotient, remainder;
        div_mod(quotient, remainder, *this, b);
        *this = quotient;
        return *this;
    }

    Num& operator %= (const Num &b){
        Num quotient, remainder;
        div_mod(quotient, remainder, *this, b);
        *this = remainder;
        return *this;
    }

    Num& operator ++ (){
        num_add_word(data, data, 1);
        return *this;
    }

    Num& operator -- (){
        num_sub_word(data, data, 1);
        return *this;
    }

    Num& set_bit(int bit_index){
        num_set_bit(data, bit_index);
        return *this;
    }

    Num& clr_bit(int bit_index){
        num_clr_bit(data, bit_index);
        return *this;
    }

    num_word get_bit(int bit_index) const {
        return num_get_bit(data, bit_index);
    }

    int bitlength() const {
        return num_bitlength(data);
    }

    int count_trailing_zeros() const {
        return num_count_trailing_zeros(data);
    }

    bool is_probable_prime(int n_tests, num_rand_func rand_func) const {
        return num_is_probable_prime(data, n_tests, rand_func);
    }

    Num sqrt() const {
        Num b;
        num_sqrt(b.data, data);
        return b;
    }

    Num pow(num_word exponent){
        Num b;
        num_pow_word(b.data, data, exponent);
        return b;
    }

    static Num gcd(const Num &a, const Num &b){
        Num c;
        num_gcd(c.data, a.data, b.data);
        return c;
    }

    static Num rand_bits(int n_bits, num_rand_func rand_func){
        Num b;
        num_rand_bits(b.data, n_bits, rand_func);
        return b;
    }

    static Num rand_inclusive(const Num &n, num_rand_func rand_func){
        Num b;
        num_rand_inclusive(b.data, n.data, rand_func);
        return b;
    }

    static Num rand_exclusive(const Num &n, num_rand_func rand_func){
        Num b;
        num_rand_exclusive(b.data, n.data, rand_func);
        return b;
    }
};

inline Num operator -(const Num &a){
    Num b(a);
    b.data->neg = !b.data->neg;
    return b;
}

inline Num operator + (const Num &a, const Num &b){
    Num c;
    num_add(c.data, a.data, b.data);
    return c;
}

inline Num operator - (const Num &a, const Num &b){
    Num c;
    num_sub(c.data, a.data, b.data);
    return c;
}

inline Num operator * (const Num &a, const Num &b){
    Num c;
    num_mul(c.data, a.data, b.data);
    return c;
}

inline Num operator / (const Num &a, const Num &b){
    Num quotient, remainder;
    Num::div_mod(quotient, remainder, a, b);
    return quotient;
}

inline Num operator % (const Num &a, const Num &b){
    Num quotient, remainder;
    Num::div_mod(quotient, remainder, a, b);
    return remainder;
}

inline Num operator << (const Num &a, int shift){
    Num b;
    num_shift_left(b.data, a.data, shift);
    return b;
}

inline Num operator >> (const Num &a, int shift){
    Num b;
    num_shift_right(b.data, a.data, shift);
    return b;
}

inline bool operator == (const Num &a, const Num &b){ return num_cmp(a.data, b.data) == 0; }
inline bool operator != (const Num &a, const Num &b){ return num_cmp(a.data, b.data) != 0; }
inline bool operator <= (const Num &a, const Num &b){ return num_cmp(a.data, b.data) <= 0; }
inline bool operator >= (const Num &a, const Num &b){ return num_cmp(a.data, b.data) >= 0; }
inline bool operator <  (const Num &a, const Num &b){ return num_cmp(a.data, b.data) <  0; }
inline bool operator >  (const Num &a, const Num &b){ return num_cmp(a.data, b.data) >  0; }

#endif
