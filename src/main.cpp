#include "fixed_string.h"
#include "utils.h"
#include <iostream>
#include <tuple>

template<typename T>
constexpr auto func_signature() {
#ifdef __clang__
    return FixedString{__PRETTY_FUNCTION__};
#elif __GNUC__
    return FixedString{__PRETTY_FUNCTION__};
#endif
}


template<typename T>
constexpr auto base_type_name() {
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

template<typename T>
constexpr auto array_type_name() {
    constexpr auto rank = std::rank_v<T>;
    constexpr auto extent = std::extent_v<T, 0>;
    if constexpr (rank == 1) {
        if constexpr (extent == 0) {
            return FixedString("[]{") + base_type_name<std::remove_extent_t<T>>() + FixedString("}");
        } else {
            return FixedString("[") + parseIntToFixedString<decltype(extent), extent>() + FixedString("]{") + base_type_name<std::remove_extent_t<T>>() + FixedString("}");
        }
    } else {
        if constexpr (extent == 0) {
            return FixedString("[]") + array_type_name<std::remove_extent_t<T>>();
        } else {
            return FixedString("[") + parseIntToFixedString<decltype(extent), extent>() + FixedString("]") + array_type_name<std::remove_extent_t<T>>();
        }
    }
}


template<typename>
struct TypeListNameHelper;
template<typename... Ts>
struct TypeListNameHelper<TypeList<Ts...>> {
    template<typename First, typename... Last>
    static constexpr auto base_types_names() {
        if constexpr (sizeof...(Last) == 0) {
            return base_type_name<First>();
        } else {
            return base_type_name<First>() + FixedString(",") + base_types_names<Last...>();
        }
    }

    static constexpr auto type_names() {
        return base_types_names<Ts...>();
    }
};

template<typename Sig>
struct FunctionSignatureHelper;
template<typename R, typename... Args>
struct FunctionSignatureHelper<R(Args...)> {
    using ArgsType = TypeList<Args...>;
    using ReturnType = R;
};

template<typename Fn>
    requires std::is_function_v<Fn>
constexpr auto function_type_name() {
    constexpr auto ArgsString = TypeListNameHelper<typename FunctionSignatureHelper<Fn>::ArgsType>::type_names();
    constexpr auto returnString = base_type_name<typename FunctionSignatureHelper<Fn>::ReturnType>();
    return concatStrings(FixedString("("), ArgsString, FixedString(")->"), returnString);
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
        return array_type_name<T>();
    } else if constexpr (std::is_function_v<T>) {
        return function_type_name<T>();
    } else {
        return base_type_name<T>();
    }
}

void func(int a, float b) {}

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
    std::cout << type_name<decltype(func)>()._data << std::endl;

    return 0;
}
