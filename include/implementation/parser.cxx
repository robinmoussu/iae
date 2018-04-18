#pragma once

#include "parsing_error.h"
#include "utility.h"

namespace iae::grammar {

template<class ...Tokens>
output_t compute(AllOf<Tokens...>) {
    return (compute(Tokens()) && ...);
};

template<class ...Tokens>
output_t compute(FirstOf<Tokens...>) {
    return (compute(Tokens()) || ...);
};

template<const char* str>
output_t compute(Token<str>) {
    return success_t{str};
};

inline
output_t compute(ErrorToken) {
    return error_t::create<ErrorToken>();
}

template<class Token1, class Token2>
constexpr auto operator&&(Token1, Token2) {
    return AllOf<Token1, Token2>{};
}

/** Memoisation */
template<class Token1, class... T>
constexpr auto operator&&(Token1, AllOf<T...>) {
    return AllOf<Token1, T...>{};
}

template<class Token1, class Token2>
constexpr auto operator||(Token1, Token2) {
    return FirstOf<Token1, Token2>{};
}

/** Memoisation */
template<class Token1, class... T>
constexpr auto operator||(Token1, FirstOf<T...>) {
    return FirstOf<Token1, T...>{};
}

}; // namespace iae::grammar
