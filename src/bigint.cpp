#include "bigint.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

bf_context_t BigInt::ctx;
bool BigInt::ctx_initialized = false;

static void *bf_realloc_godot(void *opaque, void *ptr, size_t size) {
    if (size == 0) {
        memfree(ptr);
        return nullptr;
    }
    return memrealloc(ptr, size);
}

void BigInt::_bind_methods() {
    ClassDB::bind_static_method("BigInt", D_METHOD("create", "value"), &BigInt::create);

    ClassDB::bind_static_method("BigInt", D_METHOD("plus", "a", "b"), &BigInt::plus);
    ClassDB::bind_static_method("BigInt", D_METHOD("minus", "a", "b"), &BigInt::minus);
    ClassDB::bind_static_method("BigInt", D_METHOD("multiply", "a", "b"), &BigInt::multiply);
    ClassDB::bind_static_method("BigInt", D_METHOD("divide", "a", "b"), &BigInt::divide);

    ClassDB::bind_method(D_METHOD("from_string", "value"), &BigInt::from_string);
    ClassDB::bind_method(D_METHOD("to_string"), &BigInt::to_string);
}

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

Ref<BigInt> BigInt::create(int64_t p_value) {
    Ref<BigInt> res = memnew(BigInt);
    bf_set_si(&res->value, p_value);
    return res;
}

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
    bf_div(&res->value, &a->value, &b->value, BF_PREC_INF, BF_RNDZ);
    return res;
}

void BigInt::from_string(const String &p_str) {
    CharString cs = p_str.utf8();
    const char *str = cs.get_data();
    bf_atof(&value, str, nullptr, 10, BF_PREC_INF, BF_RNDZ);
}

String BigInt::to_string() const {
    size_t len;
    char *buf = bf_ftoa(&len, &value, 10, 0, BF_RNDZ | BF_FTOA_FORMAT_FRAC);
    String result = String(buf);
    bf_free(&ctx, buf);
    return result;
}