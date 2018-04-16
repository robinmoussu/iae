#pragma once

#include "utility.h"

namespace iae::grammar {

template <class T>
ParseError ParseError::create()
{
    return ParseError(&ErrorInfo<T>::instance_v);
}

template<class T>
std::string ParseError::ErrorInfo<T>::to_string() const {
    return std::string{"Invalid token: "} + utility::print_type<T>();
}

template<class T>
ParseError::ErrorInfo<T>::~ErrorInfo() {}

template<class T>
const ParseError::ErrorInfo<T> ParseError::ErrorInfo<T>::instance_v;

}; // namespace iae::grammar
