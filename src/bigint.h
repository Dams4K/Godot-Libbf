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

    static Ref<BigInt> create(int64_t p_value);
    static Ref<BigInt> plus(const Ref<BigInt> a, const Ref<BigInt> b);
    static Ref<BigInt> minus(const Ref<BigInt> a, const Ref<BigInt> b);
    static Ref<BigInt> multiply(const Ref<BigInt> a, const Ref<BigInt> b);
    static Ref<BigInt> divide(const Ref<BigInt> a, const Ref<BigInt> b);

    void from_string(const String &p_str);
    String to_string() const;
};

}