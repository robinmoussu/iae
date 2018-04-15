#include <iostream> // TODO fmt
#include <string>
#include <array>
#include <type_traits>
#include <tuple>
#include <any>
#include <exception>
#include <regex> // TODO boost
#include <memory>
#include <variant>

#include <cxxabi.h>

//#include <string_view>

using namespace std;

////////////////////////////////////////////////////////////////////////////////

namespace utility {

// utility for pattern-matching in c++
// see http://en.cppreference.com/w/cpp/utility/variant/visit
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

class ParseError;
auto to_string(ParseError e);

template<class T>
string print_type(T t)
{
    using realtype = decltype(t);

    // if the type is std::any, return the contained type
    auto type_name = [&]() {
        if (is_same<any,realtype>::value) {
            return t.type().name();
        } else {
            return typeid(realtype).name();
        }
    }();

    auto status = 0;
    auto name = unique_ptr<char, decltype(std::free)*>{
        abi::__cxa_demangle(type_name, 0, 0, &status), std::free
    };
    if (status == 0) {
        return name.release();
    } else {
        return typeid(realtype).name();
    }
}

class ParseError {
private:
    friend auto to_string(ParseError e);
    any type_;
public:
    template<class T>
    ParseError(T t): type_(t) {}
};

auto to_string(ParseError e) {
    return string{"Invalid token: "} + print_type(e.type_);
}

}; //namespace utility

////////////////////////////////////////////////////////////////////////////////

namespace diceparser2 {

auto x = R"---(

Grammar = (Expression ";")* Expression? Output

Output =
    | "\"" (OutputStr ComputedValue?)+ "\""
    | EmptyOutput
EmptyOutput = ""
OutputStr = "([^\\$!]*(\\.)?)+"
ComputedValue =
    | PreviousResults
    | PreviousDices
    | GetResult
    | GetDices
GetResult = "$" FullExpression
GetDices = "!" FullExpression

FullExpression = "{" Expression "}"
Expression = Comment? Roll Operator+

Comment = "`" CommentStr* "`"
CommentStr =
    | "[^`]*"
    | "\`"

Quantity =
    | PreviousResults
    | Number
Number = "\d+"

PreviousResults = "$" Number
PreviousDices = "!" Number

Roll =
    | "(" (DiceOperator ",")* DiceOperator ")"
    | DiceOperator
DiceOperator =
    | PreviousDices
    | Dice
    | List

Dice = Quantity "[dD]" NbFace
NbFace = Quantity
List = Quantity "[lL]" "\[" (Item ",")* Item "\]"
Item = "\w+"

Operator =
    | KeepHigher
    | KeepLower
    | SortHigher
    | SortLower

    | MathOperation

    | TakeIf
    | AddIf
    | ExplodeIf
    | RerollIf

    | If
    | Paint

MathOperation = ArithmeticOperator Quantity
ArithmeticOperator =
    | Plus
    | Minus
    | Divide
    | Multiply
    | Modulo

Plus = "+" Quantity
Minus = "-" Quantity
Multiply = "[*×]" Quantity
Modulo = "!" Quantity
Divide = DivideRound | DivideCeil | DivideFloor | DivideReal
DivideRound = "/~" Quantity
DivideCeil = "/\+" Quantity
DivideFloor = "/\-" Quantity
DivideReal = "/" Quantity

KeepHigher = "K" Quantity
KeepLower = "k" Quantity
SortHigher = "S" Quantity
SortLower = "s" Quantity

TakeIf = "t" Condition
AddIf = "a" Condition
ExplodeIf = "e" Condition
RerollIf = "r" Condition

If = "i" (OnEach | OnAll | AtLeastOne | OnTotal) Condition TrueInstruction ElseIfInstruction* ElseInstruction

OnEach = "\+?"
OnAll = "\*"
AtLeastOne = "\."
OnTotal = ":"

TrueInstruction = FullExpression
ElseIfInstruction = Condition FullExpression
ElseInstruction = FullExpression

Condition =
    | Quantity
    | "\[" ListOperator* RelationalOperator "\]"

ListOperator = RelationalOperator (And | Or)
And = "&&?"
Or = "||?"

RelationalOperator = Superior | Inferior | SuperiorOrEqual | InferiorOrEqual | Equal | NotEqual | Range
Superior = ">" Quantity
Inferior = "<" Quantity
SuperiorOrEqual = ">=" Quantity
InferiorOrEqual = "<=" Quantity
Equal = "==?" Quantity
NotEqual = "!=" Quantity

Paint = "p\[" (Range ":")? Color

Range = (SubRange ",")* SubRange
SubRange =
    | MinOrMax
    | Quantity
MinOrMax = Min ":" Max
Min = Quantity
Max = Quantity

)---";

// Keep and Explode not kept, since 1d10K5 is equivalent to 1d10e[=10]k5
// Keep renamed TakeIf => "t"
// Sort renamed SortHigher => "S"
// added SortLower => "s"
// Add renamed AddIf
// BackwardJump and merge removed (replaced by !x and (!x, !y)
// I don't know what Group means

}; //namespace diceparser2

////////////////////////////////////////////////////////////////////////////////

namespace diceparser2::grammar {

using utility::overloaded;

using success_t = string;
using error_t   = utility::ParseError;
using output_t  = variant<success_t, grammar::error_t>;

output_t operator&& (output_t o1, output_t o2) {
    return visit(overloaded {
        [](success_t s1, success_t s2) {
            return output_t{s1 + s2};
        },
        [](success_t, error_t e) {
            return output_t{e};
        },
        [](error_t e, output_t) {
            return output_t{e};
        },
    }, o1, o2);
}

output_t operator|| (output_t o1, output_t o2) {
    return visit(overloaded {
        [](success_t s, output_t) {
            return output_t{s};
        },
        [](error_t e, output_t o) {
            return output_t{o};
        },
    }, o1, o2);
}

/** Must match all tokens */
template<class ...Tokens>
struct AllOf {};

/** Return the first matching token */
template<class ...Tokens>
struct OneOf {};

/** Regex */
template<const char* str>
struct R {};

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

string to_string(output_t o) {
    using utility::to_string;

    return visit(overloaded {
        [](error_t e) {
            return to_string(e);
        },
        [](success_t s) {
            return string{s};
        },
    }, o);
}

}; //namespace diceparser2::grammar

////////////////////////////////////////////////////////////////////////////////

namespace diceparser2 {

using namespace grammar;

const char number[] = "\\d+";
// TODO:
// PreviousResults is not implemented yet
// struct Quantity: OneOf< PreviousResults, R<number> > {};
struct Quantity: OneOf< R<number> > {};

const char divide_round[] = "/~";
const char divide_ceil[]  = "/\\+";
const char divide_floor[] = "/\\-";
const char divide_real[]  = "/";
struct DivideRound: AllOf< R<divide_round>, Quantity > {};
struct DivideCeil:  AllOf< R<divide_ceil>,  Quantity > {};
struct DivideFloor: AllOf< R<divide_floor>, Quantity > {};
struct DivideReal:  AllOf< R<divide_real>,  Quantity > {};

const char plus[] = "\\+";
const char minus[] = "-";
const char multiply[] = "[*×]";
const char modulo[] = "!";
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

}; //namespace diceparser2

////////////////////////////////////////////////////////////////////////////////

using namespace diceparser2;

constexpr auto test_me = "2d3a[=3]*1;4d6;$2*10;($1d10 + !2a[>5])*100;\"$0: $1 [ !1 ]; $2 -> $3 [ !2 -> !3 ]; $4 [ !4 ] -- !0\"";

const char a[] = "a";
const char b[] = "b";
const char c[] = "c";
const char d[] = "d";
struct A: R<a> {};
struct B: R<b> {};
struct C: R<c> {};
struct D: R<d> {};

struct AB: OneOf<A,B> {};
struct CD: AllOf<C,D> {};

using utility::overloaded;

int main(int argc, char *argv[])
{
    auto& out = cout;
    auto& err = cerr;

    const auto input = test_me;

    const auto x = output_t{success_t("xx")};
    class yy{};
    const auto y = output_t{grammar::error_t(yy{})};
    cout << "x: " << to_string(x) << endl;
    cout << "y: " << to_string(y) << endl;
    const auto x_and_y = x && y;
    cout << "x && y: " << to_string(x_and_y) << endl;
    const auto y_and_x = y && x;
    cout << "y && x: " << to_string(y_and_x) << endl;
    const auto x_or_y = x || y;
    cout << "x || y: " << to_string(x_or_y) << endl;
    const auto y_or_x = y || x;
    cout << "y || x: " << to_string(y_or_x) << endl;

    class ww{};
    const auto v = output_t{success_t("vv")};
    const auto w = output_t{grammar::error_t(ww{})};
    cout << "x && v: " << to_string(x && v) << endl;
    cout << "v && x: " << to_string(v && x) << endl;
    cout << "x && v && y && w: " << to_string(x && v && y && w) << endl;
    cout << "x || v: " << to_string(x || v) << endl;
    cout << "v || x: " << to_string(v || x) << endl;
    cout << "y || w || x || v: " << to_string(y || w || x || v) << endl;

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

    return 0;
}

