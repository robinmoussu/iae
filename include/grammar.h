#pragma once

#include "parser.h"

/* \FIXME When c++20 will be advailable, use string as non class template
 * parameter to reduce boilerplate (example: constexpr auto a_or_b = Token<"a"> || Token<"b">);
 * \see [Class Types in Non-Type Template Parameters](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0732r1.pdf))
 */

/**
 * Dice grammar
 *
 * \note Differences with DiceParser:
 * - Keep and Explode not kept, since 1d10K5 is equivalent to 1d10e[=10]k5
 * - Keep renamed TakeIf => "t"
 * - Sort renamed SortHigher => "S"
 * - added SortLower => "s"
 * - Add renamed AddIf
 * - BackwardJump and merge removed (replaced by !x and (!x, !y)
 * - Group not implemented, I don't know what they are
 */

namespace iae::grammar {

inline const char num_[] = "\\d+";
struct Number: Token<num_> {};

inline const char prev_dice_[] = "!";
struct PreviousDices: AllOf< Token<prev_dice_>, Number > {};
inline const char prev_res_[] = "$";
struct PreviousResults: AllOf< Token<prev_res_>, Number > {};

struct Quantity: FirstOf< Number, PreviousResults > {};

struct Min: Quantity {};
struct Max: Quantity {};
inline const char min_or_max_separator_[] = ":";
struct MinOrMax: FirstOf< Min, Token<min_or_max_separator_>, Max > {};
struct SubRange: FirstOf< MinOrMax, Quantity > {};
inline const char subrange_separator_[] = ",";
struct Range:
    AllOf<
        Many< FirstOf<MinOrMax,Quantity>, Token<subrange_separator_> >,
        Quantity
    > {};

inline const char paint_beg_[] = "p\\[";
inline const char paint_sep_[] = ":";
inline const char paint_end_[] = "\\]";
inline const char color_[] = "\\w+"; // FIXME: use real collor
struct Paint:
    AllOf<
        Token<paint_beg_>,
        Maybe< AllOf< Range, Token<paint_sep_> >>,
        Token<color_>,
        Token<paint_end_>
    > {};

inline const char ne_[] = "!=";
struct NotEqual: AllOf< Token<ne_>, Quantity > {};
inline const char eq_[] = "==?";
struct Equal: AllOf< Token<eq_>, Quantity > {};
inline const char le_[] = "<=";
struct InferiorOrEqual: AllOf< Token<le_>, Quantity > {};
inline const char ge_[] = ">=";
struct SuperiorOrEqual: AllOf< Token<ge_>, Quantity > {};
inline const char lt_[] = "<";
struct Inferior: AllOf< Token<lt_>, Quantity > {};
inline const char gt_[] = ">";
struct Superior: AllOf< Token<gt_>, Quantity > {};
struct RelationalOperator: FirstOf< Superior, Inferior, SuperiorOrEqual, InferiorOrEqual, Equal, NotEqual > {};

inline const char or_[] = "&&?";
struct Or: Token<or_> {};
inline const char and_[] = "&&?";
struct And: Token<and_> {};
struct ListOperator: AllOf< RelationalOperator, FirstOf< And >, Or > {};

inline const char cond_open_[] = "\\[";
inline const char cond_close_[] = "\\]";
struct Condition: FirstOf< AllOf< Token<cond_open_>, AnyOf< ListOperator >, RelationalOperator, Token<cond_close_> >, Quantity > {};

class FullExpression;
struct ElseInstruction: FirstOf<Reference<FullExpression>> {};
struct ElseIfInstruction: AllOf< Condition, Reference<FullExpression> > {};
struct TrueInstruction: FirstOf<Reference<FullExpression>> {};

inline const char separator_[] = ":";
struct OnTotal: Token<separator_> {};
inline const char alo_[] = "\\.";
struct AtLeastOne: Token<alo_> {};
inline const char oa_[] = "\\*";
struct OnAll: Token<oa_> {};
inline const char oe_[] = "\\+?";
struct OnEach: Token<oe_> {};

inline const char if_[] = "i";
struct If:
    AllOf<
        Token<if_>,
        FirstOf< OnEach, OnAll, AtLeastOne, OnTotal >,
        Condition,
        TrueInstruction,
        AnyOf< ElseIfInstruction >,
        Maybe< ElseInstruction >
    > {};

inline const char op_r_[] = "r";
struct RerollIf: AllOf< Token<op_r_>, Condition > {};
inline const char op_e_[] = "e";
struct ExplodeIf: AllOf< Token<op_e_>, Condition > {};
inline const char op_a_[] = "a";
struct AddIf: AllOf< Token<op_a_>, Condition > {};
inline const char op_t_[] = "t";
struct TakeIf: AllOf< Token<op_t_>, Condition > {};

inline const char op_s_[] = "s";
struct SortLower: AllOf< Token<op_s_>, Quantity > {};
inline const char op_S_[] = "S";
struct SortHigher: AllOf< Token<op_S_>, Quantity > {};
inline const char op_k_[] = "k";
struct KeepLower: AllOf< Token<op_k_>, Quantity > {};
inline const char op_K_[] = "K";
struct KeepHigher: AllOf< Token<op_K_>, Quantity > {};

inline const char divide_round_[] = "/~";
inline const char divide_ceil_[]  = "/\\+";
inline const char divide_floor_[] = "/\\-";
inline const char divide_real_[]  = "/";
struct DivideRound: AllOf< Token<divide_round_>, Quantity > {};
struct DivideCeil : AllOf< Token<divide_ceil_>, Quantity > {};
struct DivideFloor: AllOf< Token<divide_floor_>, Quantity > {};
struct DivideReal : AllOf< Token<divide_real_>, Quantity > {};

inline const char plus_[] = "\\+";
inline const char minus_[] = "-";
inline const char multiply_[] = "[*×]";
inline const char modulo_[] = "!";
struct Plus    : AllOf< Token<plus_>, Quantity > {};
struct Minus   : AllOf< Token<minus_>, Quantity > {};
struct Multiply: AllOf< Token<multiply_>, Quantity > {};
struct Modulo  : AllOf< Token<modulo_>, Quantity > {};
struct Divide  : FirstOf< DivideRound, DivideCeil, DivideFloor, DivideReal > {};

struct ArithmeticOperator: FirstOf< Plus, Minus, Divide, Multiply, Modulo > {};
struct MathOperation: FirstOf< ArithmeticOperator, Quantity > {};
struct Operator:
    FirstOf<
        AllOf< FirstOf< Paint, If >>,
        AllOf< FirstOf< RerollIf, ExplodeIf, AddIf, TakeIf >>,
        MathOperation,
        AllOf< FirstOf< SortLower, SortHigher, KeepLower, KeepHigher >>
    > {};

inline const char item_[] = "\\w+";
struct Item: Token<item_> {};
inline const char list_[] = "[lL]";
inline const char list_beg_[] = "\\[";
inline const char list_sep_[] = ",";
inline const char list_end_[] = "\\]";
struct List: AllOf< Quantity, Token<list_>, Token<list_beg_>, Many<Item, Token<list_sep_>>, Token<list_end_>> {};
struct NbFaces: Quantity {};
inline const char dice_[] = "[dD]";
struct Dice: AllOf< Quantity, Token<dice_>, NbFaces > {};

struct DiceOperator: FirstOf< DiceOperator, List, Dice, PreviousDices > {};
inline const char roll_beg_[] = "(";
inline const char roll_sep_[] = ",";
inline const char roll_end_[] = ")";
struct Roll: AllOf< Token<roll_beg_>, AnyOf< AllOf< DiceOperator , Token<roll_sep_>>>, DiceOperator, Token<roll_end_>> {};

inline const char escaped_quote_[] = "\\`";
inline const char comment_[] = "[^`]*";
struct CommentStr: FirstOf< Token<escaped_quote_>, Token<comment_>> {};
inline const char quote_[] = "`";
struct Comment: AllOf< Token<quote_>, AnyOf< CommentStr >, Token<quote_>> {};

struct Expression: AllOf< Maybe< Comment >, Maybe<AllOf< Roll , AnyOf< Operator >>>> {};
inline const char exp_beg_[] = "{";
inline const char exp_end_[] = "}";
inline const char expression_sep_[] = ";";
struct FullExpression: AllOf< Token<exp_beg_>, Many<Expression, Token<expression_sep_>>, Token<exp_end_>> {};

struct GetDices: AllOf< Token<prev_dice_>, FullExpression > {};
struct GetResult: AllOf< Token<prev_res_>, FullExpression > {};
struct ComputedValue: FirstOf< GetDices, GetResult, PreviousDices, PreviousResults > {};

inline const char regular_string_[] = "[^\\$!]*(\\.)?";
struct CustomOutput: AnyOf< FirstOf< Token<regular_string_>, ComputedValue >> {};
inline const char dquote_[] = "\"";
inline const char empty_[] = "";
struct EmptyOutput: Token<empty_> {};
struct OutputStr: FirstOf< EmptyOutput, CustomOutput > {};
struct Output: AllOf< Token<dquote_>, OutputStr, Token<dquote_>> {};

struct Grammar: AllOf< Many<Expression, Token<expression_sep_>>, Output> {};

}; // namespace iae::grammar
