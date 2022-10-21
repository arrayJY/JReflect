//
// Created by jy on 22-10-21.
//

#ifndef JREFLECT_UTILS_H
#define JREFLECT_UTILS_H
#include <functional>
#include <tuple>
#include <utility>

template<typename... Ts>
struct TypeList {
    struct IsTypeList {};
    using type = TypeList;

    static constexpr auto size = sizeof...(Ts);
    template<typename... T>
    using append = TypeList<Ts..., T...>;
    template<typename... T>
    using prepend = TypeList<T..., Ts...>;
};


#endif//JREFLECT_UTILS_H
