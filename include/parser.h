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
constexpr output_t compute(AllOf<Tokens...>);

/** Return the first matching token */
template<class ...Tokens>
struct OneOf {};

template<class ...Tokens>
constexpr output_t compute(OneOf<Tokens...>);

/** Regex */
template<const char* str>
struct R {};

template<const char* str>
constexpr output_t compute(R<str>);

output_t operator&& (output_t o1, output_t o2);
output_t operator|| (output_t o1, output_t o2);

/** Pretty print the output */
std::string to_string(output_t o);

}; //namespace iae::grammar

#include "implementation/parser.cxx"
