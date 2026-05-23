#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/ref.hpp>

extern "C" {
    #include "../libbf/libbf.h"
}

namespace godot {

class BigInt : public RefCounted {
    GDCLASS(BigInt, RefCounted)

private:
    bf_t value;
    static bf_context_t ctx;
    static bool ctx_initialized;

protected:
    static void _bind_methods();

public:
    BigInt();
    ~BigInt();

    // Constructeurs
    static Ref<BigInt> create(int64_t p_value);
    static Ref<BigInt> create_from_string(const String &p_str);
    
    static Ref<BigInt> plus(const Ref<BigInt> a, const Ref<BigInt> b);
    static Ref<BigInt> minus(const Ref<BigInt> a, const Ref<BigInt> b);
    static Ref<BigInt> multiply(const Ref<BigInt> a, const Ref<BigInt> b);
    static Ref<BigInt> divide(const Ref<BigInt> a, const Ref<BigInt> b);
    static Ref<BigInt> mod(const Ref<BigInt> a, const Ref<BigInt> b);
    static Ref<BigInt> minimum(const Ref<BigInt> a, const Ref<BigInt> b);
    static Ref<BigInt> maximum(const Ref<BigInt> a, const Ref<BigInt> b);
    static Ref<BigInt> absolute(const Ref<BigInt> a);

    static Ref<BigInt> plus_int(const Ref<BigInt> a, int64_t b);
    static Ref<BigInt> minus_int(const Ref<BigInt> a, int64_t b);
    static Ref<BigInt> multiply_int(const Ref<BigInt> a, int64_t b);
    static Ref<BigInt> divide_int(const Ref<BigInt> a, int64_t b);

    static bool is_equal(const Ref<BigInt> a, const Ref<BigInt> b);
    static bool is_less(const Ref<BigInt> a, const Ref<BigInt> b);
    static bool is_greater(const Ref<BigInt> a, const Ref<BigInt> b);
    static bool is_less_or_equal(const Ref<BigInt> a, const Ref<BigInt> b);
    static bool is_greater_or_equal(const Ref<BigInt> a, const Ref<BigInt> b);

    Ref<BigInt> add(const Ref<BigInt> other);
    Ref<BigInt> sub(const Ref<BigInt> other);
    Ref<BigInt> mul(const Ref<BigInt> other);
    Ref<BigInt> div(const Ref<BigInt> other);
    Ref<BigInt> add_int(int64_t p_value);
    Ref<BigInt> sub_int(int64_t p_value);
    Ref<BigInt> mul_int(int64_t p_value);
    Ref<BigInt> div_int(int64_t p_value);

    bool is_zero() const;
    bool is_negative() const;

    bool equals(const Ref<BigInt> other) const;
    bool less_than(const Ref<BigInt> other) const;
    bool greater_than(const Ref<BigInt> other) const;
    bool less_than_or_equal(const Ref<BigInt> other) const;
    bool greater_than_or_equal(const Ref<BigInt> other) const;

    void from_string(const String &p_str);
    String to_string() const;
    int64_t to_int64() const;
};

}