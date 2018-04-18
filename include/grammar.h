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
// TODO: auto Quantity = PreviousResults || Token<number>{};
constexpr auto Quantity = Token<number>{};

inline const char divide_round[] = "/~";
inline const char divide_ceil[]  = "/\\+";
inline const char divide_floor[] = "/\\-";
inline const char divide_real[]  = "/";
constexpr auto DivideRound = Token<divide_round>{} && Quantity;
constexpr auto DivideCeil  = Token<divide_ceil>{}  && Quantity;
constexpr auto DivideFloor = Token<divide_floor>{} && Quantity;
constexpr auto DivideReal  = Token<divide_real>{}  && Quantity;

inline const char plus[] = "\\+";
inline const char minus[] = "-";
inline const char multiply[] = "[*×]";
inline const char modulo[] = "!";
constexpr auto Plus     = Token<plus>{}     && Quantity;
constexpr auto Minus    = Token<minus>{}    && Quantity;
constexpr auto Multiply = Token<multiply>{} && Quantity;
constexpr auto Modulo   = Token<modulo>{}   && Quantity;
constexpr auto Divide   = DivideRound || DivideCeil || DivideFloor || DivideReal;

constexpr auto ArithmeticOperator = Plus || Minus || Divide || Multiply || Modulo;
constexpr auto MathOperation = ArithmeticOperator || Quantity;

}; // namespace iae::grammar
