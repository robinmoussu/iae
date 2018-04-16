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
    // constexpr auto test_me = R"--("2d3a[=3]*1;4d6;$2*10;($1d10 + !2a[>5])*100;"$0: $1 [ !1 ]; $2 -> $3 [ !2 -> !3 ]; $4 [ !4 ] -- !0")--";

    struct A: R<a> {};
    struct B: R<b> {};
    struct C: R<c> {};
    struct D: R<d> {};

    struct AB: OneOf<A,B> {};
    struct CD: AllOf<C,D> {};

    auto& out = cout;
    auto& err = cerr;

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

    const auto aa = compute(A{});
    cout << "a: " << to_string(aa) << endl;
    const auto allof = compute(AB{});
    cout << "ab: " << to_string(allof) << endl;
    const auto oneof = compute(CD{});
    cout << "c|d: " << to_string(oneof) << endl;

    const auto grammar_ = MathOperation{};
    const auto computed_statements = compute(grammar_);
    cout << "grammar: " << to_string(computed_statements) << endl;
    visit(overloaded {
        [&out](grammar::success_t s) { out << "Success: " << grammar::to_string(s) << endl; },
        [&err](grammar::error_t e) { err << "Failure: " << grammar::to_string(e) << endl; }
    }, computed_statements);
}

}; // namespace iae::grammar

