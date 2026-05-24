#include "bigint.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

bf_context_t BigInt::ctx;
bool BigInt::ctx_initialized = false;

static const struct { int64_t exp; const char* suffix; } SUFFIXES[] = {
    { 30, "No" },
    { 27, "Oc" },
    { 24, "Sp" },
    { 21, "Sx" },
    { 18, "Qi" },
    { 15, "Qa" },
    { 12, "T"  },
    {  9, "B"  },
    {  6, "M"  },
    {  3, "K"  },
};

static void *bf_realloc_godot(void *opaque, void *ptr, size_t size) {
    if (size == 0) {
        memfree(ptr);
        return nullptr;
    }
    return memrealloc(ptr, size);
}

// ─── Lifecycle ───────────────────────────────────────────────────────────────

BigInt::BigInt() {
    if (!ctx_initialized) {
        bf_context_init(&ctx, bf_realloc_godot, nullptr);
        ctx_initialized = true;
    }
    bf_init(&ctx, &value);
}

BigInt::~BigInt() {
    bf_delete(&value);   
}

// ─── Constructors ────────────────────────────────────────────────────────────

Ref<BigInt> BigInt::create(int64_t p_value) {
    Ref<BigInt> res = memnew(BigInt);
    bf_set_si(&res->value, p_value);
    return res;
}

Ref<BigInt> BigInt::create_from_string(const String &p_str) {
    Ref<BigInt> res = memnew(BigInt);
    res->from_string(p_str);
    return res;
}

// ─── Conversion ──────────────────────────────────────────────────────────────

void BigInt::from_string(const String &p_str) {
    CharString cs = p_str.utf8();
    bf_atof(&value, cs.get_data(), nullptr, 10, BF_PREC_INF, BF_RNDZ);
}

String BigInt::to_string() const {
    size_t len;
    char *buf = bf_ftoa(&len, &value, 10, 0, BF_RNDZ | BF_FTOA_FORMAT_FRAC);
    String result = String(buf);
    bf_free(&ctx, buf);
    return result;
}

String BigInt::to_scientific(int decimals) const {
    size_t len;
    char *buf = bf_ftoa(&len, &value, 10, decimals, 
                        BF_RNDZ | BF_FTOA_FORMAT_FIXED | BF_FTOA_FORCE_EXP);
    String result = String(buf);
    bf_free(&ctx, buf);
    return result;
}

String BigInt::to_metrics(int digits) const {
    if (this->is_zero()) return String("0");

    String str = this->to_string();
    bool negative = this->is_negative();
    if (negative) str = str.substr(1);
    int64_t num_digits = str.length();
    str = str.substr(0, digits);

    for (auto& suffix : SUFFIXES) {
        if (num_digits <= suffix.exp) continue;
        
        int int_len = num_digits - suffix.exp;
        
        String int_part = str.substr(0, int_len);
        String dec_part = str.substr(int_len);

        String result = int_part;
        if (!dec_part.is_empty()) {
            result += "." + dec_part;   
        }
        result += suffix.suffix;
        return (negative ? "-" : "") + result;
    }

    return (negative ? "-" : "") + str;
}

int64_t BigInt::to_int64() const {
    int64_t result = 0;
    bf_get_int64(&result, &value, 0);
    return result;
}

// ─── Static operators ────────────────────────────────────────────────────────

Ref<BigInt> BigInt::plus(Ref<BigInt> a, Ref<BigInt> b) {
    Ref<BigInt> res = memnew(BigInt);
    bf_add(&res->value, &a->value, &b->value, BF_PREC_INF, BF_RNDZ);
    return res;
}

Ref<BigInt> BigInt::minus(Ref<BigInt> a, Ref<BigInt> b) {
    Ref<BigInt> res = memnew(BigInt);
    bf_sub(&res->value, &a->value, &b->value, BF_PREC_INF, BF_RNDZ);
    return res;
}

Ref<BigInt> BigInt::multiply(Ref<BigInt> a, Ref<BigInt> b) {
    Ref<BigInt> res = memnew(BigInt);
    bf_mul(&res->value, &a->value, &b->value, BF_PREC_INF, BF_RNDZ);
    return res;
}

Ref<BigInt> BigInt::divide(Ref<BigInt> a, Ref<BigInt> b) {
    Ref<BigInt> res = memnew(BigInt);
    bf_t remainder;
    bf_init(&ctx, &remainder);
    bf_divrem(&res->value, &remainder, &a->value, &b->value, BF_PREC_INF, BF_RNDZ, BF_RNDD);
    bf_delete(&remainder); // In an int, we don't want a remainder
    return res;
}

Ref<BigInt> BigInt::mod(const Ref<BigInt> a, const Ref<BigInt> b) {
    Ref<BigInt> res = memnew(BigInt);
    bf_t quotient;
    bf_init(&ctx, &quotient);
    bf_divrem(&quotient, &res->value, &a->value, &b->value, BF_PREC_INF, BF_RNDZ, BF_RNDD);
    bf_delete(&quotient);
    return res;
}

Ref<BigInt> BigInt::minimum(const Ref<BigInt> a, const Ref<BigInt> b) {
    return bf_cmp(&a->value, &b->value) <= 0 ? a : b;
}

Ref<BigInt> BigInt::maximum(const Ref<BigInt> a, const Ref<BigInt> b) {
    return bf_cmp(&a->value, &b->value) >= 0 ? a : b;
}

Ref<BigInt> BigInt::absolute(const Ref<BigInt> a) {
    Ref<BigInt> res = memnew(BigInt);
    bf_set(&res->value, &a->value);
    if (res->value.sign) res->value.sign = 0;
    return res;
}

// ─── Operators with int64 ────────────────────────────────────────────────────

Ref<BigInt> BigInt::plus_int(const Ref<BigInt> a, int64_t b) {
    Ref<BigInt> res = memnew(BigInt);
    bf_add_si(&res->value, &a->value, b, BF_PREC_INF, BF_RNDZ);
    return res;
}

Ref<BigInt> BigInt::minus_int(const Ref<BigInt> a, int64_t b) {
    Ref<BigInt> res = memnew(BigInt);
    bf_t subtrahend;
    bf_init(&ctx, &subtrahend);
    bf_set_si(&subtrahend, b);
    bf_sub(&res->value, &a->value, &subtrahend, BF_PREC_INF, BF_RNDZ);
    bf_delete(&subtrahend);
    return res;
}

Ref<BigInt> BigInt::multiply_int(const Ref<BigInt> a, int64_t b) {
    Ref<BigInt> res = memnew(BigInt);
    bf_mul_si(&res->value, &a->value, b, BF_PREC_INF, BF_RNDZ);
    return res;
}

Ref<BigInt> BigInt::divide_int(const Ref<BigInt> a, int64_t b) {
    Ref<BigInt> res = memnew(BigInt);
    bf_t divisor;
    bf_init(&ctx, &divisor);
    bf_set_si(&divisor, b);
    bf_t remainder;
    bf_init(&ctx, &remainder);
    bf_divrem(&res->value, &remainder, &a->value, &divisor, BF_PREC_INF, BF_RNDZ, BF_RNDD);
    bf_delete(&remainder);
    bf_delete(&divisor);
    return res;
}

bool BigInt::is_equal(const Ref<BigInt> a, const Ref<BigInt> b) {
    return bf_cmp(&a->value, &b->value) == 0;
}

bool BigInt::is_less(const Ref<BigInt> a, const Ref<BigInt> b) {
    return bf_cmp(&a->value, &b->value) < 0;
}

bool BigInt::is_greater(const Ref<BigInt> a, const Ref<BigInt> b) {
    return bf_cmp(&a->value, &b->value) > 0;
}

bool BigInt::is_less_or_equal(const Ref<BigInt> a, const Ref<BigInt> b) {
    return bf_cmp(&a->value, &b->value) <= 0;
}

bool BigInt::is_greater_or_equal(const Ref<BigInt> a, const Ref<BigInt> b) {
    return bf_cmp(&a->value, &b->value) >= 0;
}

// ─── Mutating operations ─────────────────────────────────────────────────────

Ref<BigInt> BigInt::add(const Ref<BigInt> other) {
    bf_add(&value, &value, &other->value, BF_PREC_INF, BF_RNDZ);
    return Ref<BigInt>(this);
}

Ref<BigInt> BigInt::sub(const Ref<BigInt> other) {
    bf_sub(&value, &value, &other->value, BF_PREC_INF, BF_RNDZ);
    return Ref<BigInt>(this);
}

Ref<BigInt> BigInt::mul(const Ref<BigInt> other) {
    bf_mul(&value, &value, &other->value, BF_PREC_INF, BF_RNDZ);
    return Ref<BigInt>(this);
}

Ref<BigInt> BigInt::div(const Ref<BigInt> other) {
    bf_t remainder;
    bf_init(&ctx, &remainder);
    bf_t quotient;
    bf_init(&ctx, &quotient);
    bf_divrem(&quotient, &remainder, &value, &other->value, BF_PREC_INF, BF_RNDZ, BF_RNDD);
    bf_move(&value, &quotient);
    bf_delete(&remainder);
    return Ref<BigInt>(this);
}

Ref<BigInt> BigInt::add_int(int64_t p_value) {
    bf_add_si(&value, &value, p_value, BF_PREC_INF, BF_RNDZ);
    return Ref<BigInt>(this);
}

Ref<BigInt> BigInt::sub_int(int64_t p_value) {
    bf_t subtrahend;
    bf_init(&ctx, &subtrahend);
    bf_set_si(&subtrahend, p_value);
    bf_sub(&value, &value, &subtrahend, BF_PREC_INF, BF_RNDZ);
    bf_delete(&subtrahend);
    return Ref<BigInt>(this);
}

Ref<BigInt> BigInt::mul_int(int64_t p_value) {
    bf_mul_si(&value, &value, p_value, BF_PREC_INF, BF_RNDZ);
    return Ref<BigInt>(this);
}

Ref<BigInt> BigInt::div_int(int64_t b) {
    bf_t divisor;
    bf_init(&ctx, &divisor);
    bf_set_si(&divisor, b);

    bf_t remainder;
    bf_init(&ctx, &remainder);
    bf_t quotient;
    bf_init(&ctx, &quotient);
    bf_divrem(&quotient, &remainder, &value, &divisor, BF_PREC_INF, BF_RNDZ, BF_RNDD);
    bf_move(&value, &quotient);

    bf_delete(&remainder);
    bf_delete(&divisor);
    return Ref<BigInt>(this);
}

// ─── Comparisons ─────────────────────────────────────────────────────────────

bool BigInt::equals(const Ref<BigInt> other) const {
    return bf_cmp(&value, &other->value) == 0;
}

bool BigInt::less_than(const Ref<BigInt> other) const {
    return bf_cmp(&value, &other->value) < 0;
}

bool BigInt::greater_than(const Ref<BigInt> other) const {
    return bf_cmp(&value, &other->value) > 0;
}

bool BigInt::less_than_or_equal(const Ref<BigInt> other) const {
    return bf_cmp(&value, &other->value) <= 0;
}

bool BigInt::greater_than_or_equal(const Ref<BigInt> other) const {
    return bf_cmp(&value, &other->value) >= 0;
}

bool BigInt::is_zero() const {
    return bf_is_zero(&value);
}

bool BigInt::is_negative() const {
    return value.sign != 0 && !bf_is_zero(&value);
}

void BigInt::_bind_methods() {
    ClassDB::bind_static_method("BigInt", D_METHOD("create", "value"), &BigInt::create);
    ClassDB::bind_static_method("BigInt", D_METHOD("create_from_string", "value"), &BigInt::create_from_string);

    ClassDB::bind_method(D_METHOD("from_string", "value"), &BigInt::from_string);
    ClassDB::bind_method(D_METHOD("to_string"), &BigInt::to_string);
    ClassDB::bind_method(D_METHOD("to_scientific", "decimals"), &BigInt::to_scientific);
    ClassDB::bind_method(D_METHOD("to_metrics", "digits"), &BigInt::to_metrics);
    ClassDB::bind_method(D_METHOD("to_int64"), &BigInt::to_int64);

    ClassDB::bind_static_method("BigInt", D_METHOD("plus", "a", "b"), &BigInt::plus);
    ClassDB::bind_static_method("BigInt", D_METHOD("minus", "a", "b"), &BigInt::minus);
    ClassDB::bind_static_method("BigInt", D_METHOD("multiply", "a", "b"), &BigInt::multiply);
    ClassDB::bind_static_method("BigInt", D_METHOD("divide", "a", "b"), &BigInt::divide);
    ClassDB::bind_static_method("BigInt", D_METHOD("mod", "a", "b"), &BigInt::mod);
    ClassDB::bind_static_method("BigInt", D_METHOD("minimum", "a", "b"), &BigInt::minimum);
    ClassDB::bind_static_method("BigInt", D_METHOD("maximum", "a", "b"), &BigInt::maximum);
    ClassDB::bind_static_method("BigInt", D_METHOD("absolute", "a"), &BigInt::absolute);

    ClassDB::bind_static_method("BigInt", D_METHOD("plus_int", "a", "b"), &BigInt::plus_int);
    ClassDB::bind_static_method("BigInt", D_METHOD("minus_int", "a", "b"), &BigInt::minus_int);
    ClassDB::bind_static_method("BigInt", D_METHOD("multiply_int", "a", "b"), &BigInt::multiply_int);
    ClassDB::bind_static_method("BigInt", D_METHOD("divide_int", "a", "b"), &BigInt::divide_int);

    ClassDB::bind_static_method("BigInt", D_METHOD("is_equal", "a", "b"), &BigInt::is_equal);
    ClassDB::bind_static_method("BigInt", D_METHOD("is_less", "a", "b"), &BigInt::is_less);
    ClassDB::bind_static_method("BigInt", D_METHOD("is_greater", "a", "b"), &BigInt::is_greater);
    ClassDB::bind_static_method("BigInt", D_METHOD("is_less_or_equal", "a", "b"), &BigInt::is_less_or_equal);
    ClassDB::bind_static_method("BigInt", D_METHOD("is_greater_or_equal", "a", "b"), &BigInt::is_greater_or_equal);

    ClassDB::bind_method(D_METHOD("add", "other"), &BigInt::add);
    ClassDB::bind_method(D_METHOD("sub", "other"), &BigInt::sub);
    ClassDB::bind_method(D_METHOD("mul", "other"), &BigInt::mul);
    ClassDB::bind_method(D_METHOD("div", "other"), &BigInt::div);

    ClassDB::bind_method(D_METHOD("add_int", "value"), &BigInt::add_int);
    ClassDB::bind_method(D_METHOD("sub_int", "value"), &BigInt::sub_int);
    ClassDB::bind_method(D_METHOD("mul_int", "value"), &BigInt::mul_int);
    ClassDB::bind_method(D_METHOD("div_int", "value"), &BigInt::div_int);

    ClassDB::bind_method(D_METHOD("equals", "other"), &BigInt::equals);
    ClassDB::bind_method(D_METHOD("less_than", "other"), &BigInt::less_than);
    ClassDB::bind_method(D_METHOD("greater_than", "other"), &BigInt::greater_than);
    ClassDB::bind_method(D_METHOD("less_than_or_equal", "other"), &BigInt::less_than_or_equal);
    ClassDB::bind_method(D_METHOD("greater_than_or_equal", "other"), &BigInt::greater_than_or_equal);

    ClassDB::bind_method(D_METHOD("is_zero"), &BigInt::is_zero);
    ClassDB::bind_method(D_METHOD("is_negative"), &BigInt::is_negative);
}