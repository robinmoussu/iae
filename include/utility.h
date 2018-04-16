#pragma once

#include <string>

namespace iae::utility {

/**
 * Convert a type T into a string.
 *
 * Use the cxxabi to convert mangled names into readable string, and fall back
 * to mangled names if not possible.
 */
template<class T>
std::string print_type();

/**
 * utility for pattern-matching in c++
 * see http://en.cppreference.com/w/cpp/utility/variant/visit
 */
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

}; // namespace iae::utility

#include "implementation/utility.cxx"
