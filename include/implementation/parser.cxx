#pragma once

#include "parsing_error.h"
#include "utility.h"

namespace iae::grammar {

template<class ...Tokens>
constexpr output_t compute(AllOf<Tokens...>) {
    return (compute(Tokens()) && ...);
};

template<class ...Tokens>
constexpr output_t compute(OneOf<Tokens...>) {
    return (compute(Tokens()) || ...);
};

template<const char* str>
constexpr output_t compute(R<str>) {
    return success_t{str};
};

}; // namespace iae::grammar
