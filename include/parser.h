#pragma once

#include "utility.h"
#include "parsing_error.h"

#include <limits>
#include <string>
#include <variant>

namespace iae::grammar {

using success_t = std::string;
using error_t   = grammar::ParseError;
using output_t  = std::variant<success_t, grammar::error_t>;

/** Must match all tokens */
template<class ...Tokens>
struct AllOf {};

template<class ...Tokens>
inline output_t compute(AllOf<Tokens...>);

/** Return the first matching token */
template<class ...Tokens>
struct FirstOf {};

template<class ...Tokens>
inline output_t compute(FirstOf<Tokens...>);

/** Return between Min and Max Tokens */
template<unsigned Min, unsigned Max, class Token>
struct Match {
    Match() = default;
    constexpr Match<Min, Max,Token>(Token&&): Match() {}
};
constexpr unsigned match_any = std::numeric_limits<unsigned>::max();

template<unsigned Min, unsigned Max, class Token>
inline output_t compute(Match<Min,Max,Token>);

template<class Token>
struct AnyOf: Match<0, match_any, Token> {
    AnyOf() = default;
    template<class T>
    constexpr AnyOf(T): AnyOf() {}
};

template<class Token, class Separator>
struct Many: AllOf< AnyOf< AllOf< Token, Separator >>, Token > {
    Many() = default;
    template<class T>
    constexpr Many(T): Many() {}
};

template<class Token>
struct MultipleOf: Match<1, match_any, Token> {
    MultipleOf() = default;
    template<class T>
    constexpr MultipleOf(T): MultipleOf() {}
};

/** When you need to have loop in your grammar */
template<class Token>
struct Reference {};

template<class Token>
inline output_t compute(Reference<Token>);

template<class Token>
struct Maybe: Match<0, 1, Token> {
    Maybe() = default;
    template<class T>
    constexpr Maybe(T): Maybe() {}
};

/** Basic token, must match the regex `str`
 *
 * \FIXME When c++20 will be advailable, use user defined literals to reduce
 * bolerblate (example: `constexpr auto a_or_b = "a"_t || "b"_t`)
 * \see [Class Types in Non-Type Template Parameters](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0732r1.pdf))
 */
template<const char* str>
struct Token {};

template<const char* str>
inline output_t compute(Token<str>);

/** Token always return false when computed. For testing purpose */
struct ErrorToken {};

inline output_t compute(ErrorToken);

inline output_t operator+ (output_t o1, output_t o2);
inline output_t operator&& (output_t o1, output_t o2);
inline output_t operator|| (output_t o1, output_t o2);

/** Concatenation with natural syntax **/
template<class Token1, class Token2>
inline constexpr auto operator&&(Token1, Token2);

/** Union with natural syntax **/
template<class Token1, class Token2>
inline constexpr auto operator||(Token1, Token2);

/** Pretty print the output */
std::string to_string(output_t o);

}; //namespace iae::grammar

#include "implementation/parser.cxx"
