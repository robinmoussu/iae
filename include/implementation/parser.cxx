#pragma once

#include "parsing_error.h"
#include "utility.h"

namespace iae::grammar {

template<class ...Tokens>
inline output_t compute(AllOf<Tokens...>) {
    return (compute(Tokens()) && ...);
};

template<class ...Tokens>
inline output_t compute(FirstOf<Tokens...>) {
    return (compute(Tokens()) || ...);
};

namespace {
// Used when a pattern match 0 times
const char* null_token = "";
};

template<unsigned Min, unsigned Max, class Token>
inline output_t compute(Match<Min, Max, Token>) {
    return visit(iae::utility::overloaded {
        [](success_t s) {
            return output_t{s} + compute(Match<Min-1, Max-1, Token>{});
        },
        [](error_t e) {
            return output_t{e};
        },
    }, compute(Token{}));
};

template<class Token, unsigned Max>
inline output_t compute(Match<1, Max, Token>) {
    return visit(iae::utility::overloaded {
        [](success_t s) {
            return output_t{s} + compute(Match<1, Max-1, Token>{});
        },
        [](error_t e) {
            return output_t{e};
        },
    }, compute(Token{}));
};

template<class Token>
inline output_t compute(Match<1, 1, Token>) {
    return visit(iae::utility::overloaded {
        [](success_t s) {
            return output_t{s};
        },
        [](error_t e) {
            return output_t{e};
        },
    }, compute(Token{}));
};

template<class Token, unsigned Max>
inline output_t compute(Match<0, Max, Token>) {
    return visit(iae::utility::overloaded {
        [](success_t s) {
            return output_t{s} + compute(Match<0, Max-1, Token>{});
        },
        [](error_t e) {
            return output_t{success_t{null_token}};
        },
    }, compute(Token{}));
};

template<class Token>
inline output_t compute(Match<0, 0, Token>) {
    return visit(iae::utility::overloaded {
        [](success_t s) {
            return output_t{s};
        },
        [](error_t e) {
            return output_t{success_t{null_token}};
        },
    }, compute(Token{}));
};

template<class Token>
AnyOf(Token) -> AnyOf<Token>;

template<class Token, class Separator>
Many(Token, Separator) -> Many<Token, Separator>;

template<class Token>
MultipleOf(Token) -> MultipleOf<Token>;

template<class Token>
Maybe(Token) -> Maybe<Token>;

template<class Token>
Match(Token) -> Match<1, 1, Token>;

template<class Token>
inline output_t compute(Reference<Token>) {
    return compute(Token{});
}

template<const char* str>
inline output_t compute(Token<str>) {
    return success_t{str};
};

inline output_t compute(ErrorToken) {
    return error_t::create<ErrorToken>();
}

inline output_t operator+ (output_t o1, output_t o2) {
    return visit(iae::utility::overloaded {
        [](success_t s1, success_t s2) {
            return output_t{s1 + s2};
        },
        [](success_t s, error_t) {
            return output_t{s};
        },
        [](error_t e, output_t) {
            return output_t{e};
        },
    }, o1, o2);
}

inline output_t operator&& (output_t o1, output_t o2) {
    return visit(iae::utility::overloaded {
        [](success_t s1, success_t s2) {
            return output_t{s1 + s2};
        },
        [](success_t, error_t e) {
            return output_t{e};
        },
        [](error_t e, output_t) {
            return output_t{e};
        },
    }, o1, o2);
}

inline output_t operator|| (output_t o1, output_t o2) {
    return visit(iae::utility::overloaded {
        [](success_t s, output_t) {
            return output_t{s};
        },
        [](error_t e, output_t o) {
            return output_t{o};
        },
    }, o1, o2);
}

template<class Token1, class Token2>
inline constexpr auto operator&&(Token1, Token2) {
    return AllOf<Token1, Token2>{};
}

/** Memoisation */
template<class Token1, class... T>
inline constexpr auto operator&&(Token1, AllOf<T...>) {
    return AllOf<Token1, T...>{};
}

template<class Token1, class Token2>
inline constexpr auto operator||(Token1, Token2) {
    return FirstOf<Token1, Token2>{};
}

/** Memoisation */
template<class Token1, class... T>
inline constexpr auto operator||(Token1, FirstOf<T...>) {
    return FirstOf<Token1, T...>{};
}

}; // namespace iae::grammar
