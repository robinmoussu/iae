#pragma once

#include "parser.h"

/* \file
 * Current (incomplete) implementation of the grammar
 *
 * \FIXME When c++20 will be advailable, use string as non class template
 * parameter to reduce boilerplate
 * \see [Class Types in Non-Type Template Parameters](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0732r1.pdf))
 */

namespace iae::grammar {

// Note: Complete grammar isn't implemented currently

inline const char number[] = "\\d+";
// TODO: struct Quantity: OneOf< PreviousResults, R<number> > {};
struct Quantity: OneOf< R<number> > {};

inline const char divide_round[] = "/~";
inline const char divide_ceil[]  = "/\\+";
inline const char divide_floor[] = "/\\-";
inline const char divide_real[]  = "/";
struct DivideRound: AllOf< R<divide_round>, Quantity > {};
struct DivideCeil:  AllOf< R<divide_ceil>,  Quantity > {};
struct DivideFloor: AllOf< R<divide_floor>, Quantity > {};
struct DivideReal:  AllOf< R<divide_real>,  Quantity > {};

inline const char plus[] = "\\+";
inline const char minus[] = "-";
inline const char multiply[] = "[*×]";
inline const char modulo[] = "!";
struct Plus:     AllOf< R<plus>,     Quantity > {};
struct Minus:    AllOf< R<minus>,    Quantity > {};
struct Multiply: AllOf< R<multiply>, Quantity > {};
struct Modulo:   AllOf< R<modulo>,   Quantity > {};
struct Divide:
    OneOf<
        DivideRound,
        DivideCeil,
        DivideFloor,
        DivideReal
    > {};

struct ArithmeticOperator:
    OneOf<
        Plus,
        Minus,
        Divide,
        Multiply,
        Modulo
    > {};
struct MathOperation:
    OneOf<
        ArithmeticOperator,
        Quantity
    > {};

}; // namespace iae::grammar
