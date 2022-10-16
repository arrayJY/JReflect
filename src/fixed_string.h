//
// Created by arrayJY on 2022/10/16.
//

#ifndef JREFLECT_FIXED_STRING_H
#define JREFLECT_FIXED_STRING_H
#include <algorithm>

template<typename CharT, std::size_t N>
struct FixedString {
    explicit constexpr FixedString() = default;
    explicit constexpr FixedString(const CharT (&s)[N]) {
        std::copy(s, s + N, _data);
    }

    constexpr auto operator<=>(const FixedString &) const = default;

    template<std::size_t M>
    constexpr auto operator+(const FixedString<CharT, M> &s) const {
        auto newString = FixedString<CharT, N + M - 1>();
        std::copy(_data, _data + N, newString._data);
        std::copy(s._data, s._data + M, newString._data + N - 1);
        return newString;
    }


    template<std::size_t start, std::size_t end = N>
    [[nodiscard]] constexpr auto substr() const {
        constexpr auto size = end - start;
        auto newString = FixedString<CharT, size>();
        std::copy(_data + start, _data + end, newString._data);
        newString._data[size - 1] = '\0';
        return newString;
    }

    static constexpr auto size() {
        return N;
    }
    static constexpr auto len() {
        return size() - 1;
    }

    CharT _data[N];
};

#endif//JREFLECT_FIXED_STRING_H
