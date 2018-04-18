#pragma once

#include "utility.h"
#include "parsing_error.h"

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
output_t compute(AllOf<Tokens...>);

/** Return the first matching token */
template<class ...Tokens>
struct FirstOf {};

template<class ...Tokens>
output_t compute(FirstOf<Tokens...>);

/** Basic token, must match the regex str */
template<const char* str>
struct Token {};

template<const char* str>
output_t compute(Token<str>);

/** Token always return false when computed. For testing purpose */
struct ErrorToken {};

output_t compute(ErrorToken);

output_t operator&& (output_t o1, output_t o2);
output_t operator|| (output_t o1, output_t o2);

/** Concatenation with natural syntax **/
template<class Token1, class Token2>
constexpr auto operator&&(Token1, Token2);

/** Union with natural syntax **/
template<class Token1, class Token2>
constexpr auto operator||(Token1, Token2);

/** Pretty print the output */
std::string to_string(output_t o);

}; //namespace iae::grammar

#include "implementation/parser.cxx"
