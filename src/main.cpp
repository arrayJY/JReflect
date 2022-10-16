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
    } else if constexpr (std::is_array_v<T>) {
        constexpr auto rank = std::rank_v<T>;
        constexpr auto extent = std::extent_v<T, 0>;
        if constexpr (rank == 1) {
            if constexpr (extent == 0) {
                return FixedString("[]{") + type_name<std::remove_extent_t<T>>() + FixedString("}");
            } else {
                return FixedString("[") + parseIntToFixedString<decltype(extent), extent>() + FixedString("]{") + type_name<std::remove_extent_t<T>>() + FixedString("}");
            }
        } else {
            if constexpr (extent == 0) {
                return FixedString("[]") + type_name<std::remove_extent_t<T>>();
            } else {
                return FixedString("[") + parseIntToFixedString<decltype(extent), extent>() + FixedString("]") + type_name<std::remove_extent_t<T>>();
            }
        }
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
    std::cout << type_name<int[][2][12]>()._data << std::endl;

    return 0;
}
