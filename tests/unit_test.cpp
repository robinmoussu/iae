#include "parser.h"
#include "grammar.h"
#include "utility.h"

#include <iostream>
#include <string>

using namespace std;
using namespace iae::grammar;
using namespace iae::utility;

namespace iae::grammar {

constexpr char a[] = "a";
constexpr char b[] = "b";
constexpr char c[] = "c";
constexpr char d[] = "d";

void unit_test()
{
    // constexpr auto test_me = Token"--("2d3a[=3]*1;4d6;$2*10;($1d10 + !2a[>5])*100;"$0: $1 [ !1 ]; $2 -> $3 [ !2 -> !3 ]; $4 [ !4 ] -- !0")--";

    class Error_0{};
    const auto success_0 = output_t{success_t("success_0")};
    const auto error_0 = output_t{error_t::create<Error_0>()};
    cout << "success_0: " << to_string(success_0) << endl;
    cout << "error_0: " << to_string(error_0) << endl;
    const auto x_and_y = success_0 && error_0;
    cout << "success_0 && error_0: " << to_string(x_and_y) << endl;
    const auto y_and_x = error_0 && success_0;
    cout << "error_0 && success_0: " << to_string(y_and_x) << endl;
    const auto x_or_y = success_0 || error_0;
    cout << "success_0 || error_0: " << to_string(x_or_y) << endl;
    const auto y_or_x = error_0 || success_0;
    cout << "error_0 || success_0: " << to_string(y_or_x) << endl;

    class Error_1{};
    const auto success_1 = output_t{success_t("success_1")};
    const auto error_1 = output_t{grammar::error_t::create<Error_1>()};
    cout << "success_0 && success_1: " << to_string(success_0 && success_1) << endl;
    cout << "success_1 && success_0: " << to_string(success_1 && success_0) << endl;
    cout << "success_0 && success_1 && error_0 && error_1: " << to_string(success_0 && success_1 && error_0 && error_1) << endl;
    cout << "success_0 && success_1 && error_0 && error_1: " << to_string(success_0 && success_1 && error_0 && error_1) << endl;
    cout << "success_0 || success_1: " << to_string(success_0 || success_1) << endl;
    cout << "success_1 || success_0: " << to_string(success_1 || success_0) << endl;
    cout << "error_0 || error_1 || success_0 || success_1: " << to_string(error_0 || error_1 || success_0 || success_1) << endl;
    cout << "error_0 || error_1 || success_1 || success_0: " << to_string(error_0 || error_1 || success_1 || success_0) << endl;

    constexpr auto A = Token<a>{};
    constexpr auto B = Token<b>{};
    constexpr auto C = Token<c>{};
    constexpr auto D = Token<d>{};

    cout << "A: " << to_string(compute(A)) << endl;
    cout << "A && B: " << to_string(compute(A && B)) << endl;
    cout << "B && A: " << to_string(compute(B && A)) << endl;
    cout << "C || D: " << to_string(compute(C || D)) << endl;
    cout << "D || C: " << to_string(compute(D || C)) << endl;
    cout << "A && B && C && D: " << to_string(compute(A && B && C && D)) << endl;
    cout << "(A && B) && (C && D): " << to_string(compute((A && B) && (C && D))) << endl;
    cout << "(A || B) || (C || D): " << to_string(compute((A || B) || (C || D))) << endl;
    cout << "A || B || C || D: " << to_string(compute(A || B || C || D)) << endl;

    constexpr auto Invalid = ErrorToken{};
    cout << "Invalid: " << to_string(compute(Invalid)) << endl;
    cout << "A && Invalid: " << to_string(compute(A && Invalid)) << endl;
    cout << "Invalid && A: " << to_string(compute(Invalid && A)) << endl;
    cout << "A || Invalid: " << to_string(compute(A || Invalid)) << endl;
    cout << "Invalid || A: " << to_string(compute(Invalid || A)) << endl;
    cout << "A && B && C && Invalid && D: " << to_string(compute(A && B && C && Invalid && D)) << endl;
    cout << "A || B || C || Invalid || D: " << to_string(compute(A || B || C || Invalid || D)) << endl;
    cout << "(A && B) && (C && Invalid && D): " << to_string(compute((A && B) && (C && Invalid && D))) << endl;
    cout << "(A || B) || (C || Invalid || D): " << to_string(compute((A || B) || (C || Invalid || D))) << endl;
    cout << "(A && B) || (C && Invalid && D): " << to_string(compute((A && B) || (C && Invalid && D))) << endl;
    cout << "(A && B && Invalid) || (C && D): " << to_string(compute((A && B && Invalid) || (C && D))) << endl;

    const auto grammar_ = MathOperation{};
    cout << "grammar: " << print_type<MathOperation>();
    const auto computed_statements = compute(grammar_);
    cout << "grammar: " << to_string(computed_statements) << endl;
    visit(overloaded {
        [](grammar::success_t s) { cout << "Success: " << grammar::to_string(s) << endl; },
        [](grammar::error_t e)   { cerr << "Failure: " << grammar::to_string(e) << endl; }
    }, computed_statements);
}

}; // namespace iae::grammar

