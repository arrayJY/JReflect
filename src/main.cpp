#include "fixed_string.h"
#include <iostream>

template<typename T>
constexpr auto func_signature() {
#ifdef __clang__
    return FixedString{__PRETTY_FUNCTION__};
#elif __GNUC__
    return FixedString{__PRETTY_FUNCTION__};
#endif
}


template<typename T>
constexpr auto type_name() {
    if constexpr (std::is_lvalue_reference_v<T>) {
        return FixedString("&{") + type_name<std::remove_reference_t<T>>() + FixedString("}");
    } else if constexpr (std::is_rvalue_reference_v<T>) {
        return FixedString("&&{") + type_name<std::remove_reference_t<T>>() + FixedString("}");
    } else if constexpr (std::is_const_v<T> && std::is_volatile_v<T>) {
        return FixedString("const volatile{") + type_name<std::remove_cv_t<T>>() + FixedString("}");
    } else if constexpr (std::is_const_v<T> && !std::is_volatile_v<T>) {
        return FixedString("const{") + type_name<std::remove_cv_t<T>>() + FixedString("}");
    } else if constexpr (std::is_pointer_v<T>) {
        return FixedString("*{") + type_name<std::remove_pointer_t<T>>() + FixedString("}");
    } else {
        constexpr auto signature = func_signature<T>();
        constexpr auto prefix = FixedString(
#ifdef __clang__
                "auto func_signature() [T = "
#elif __GNUC__
                "constexpr auto func_signature() [with T = "
#endif
        );
        constexpr auto suffix = FixedString("]");
        constexpr auto start = decltype(prefix)::len();
        constexpr auto end = decltype(signature)::size() - decltype(suffix)::len();
        return signature.template substr<start, end>();
    }
}


int main(int argc, char **argv) {
    std::cout << type_name<int>()._data << std::endl;
    std::cout << type_name<const int>()._data << std::endl;
    std::cout << type_name<const volatile int>()._data << std::endl;
    std::cout << type_name<int &>()._data << std::endl;
    std::cout << type_name<const int &>()._data << std::endl;
    std::cout << type_name<const int &&>()._data << std::endl;
    std::cout << type_name<int *>()._data << std::endl;
    std::cout << type_name<const int *>()._data << std::endl;

    return 0;
}
