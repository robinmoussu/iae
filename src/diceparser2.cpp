#include <iostream> // TODO fmt
#include <string>
#include <array>
#include <type_traits>
#include <tuple>
#include <any>
#include <exception>
#include <regex> // TODO boost
#include <memory>

#include <cxxabi.h>

//#include <string_view>

using namespace std;

namespace diceparser2 {

////////////////////////////////////////////////////////////////////////////////

template<typename T>
string print_type()
{
    auto status = 0;
    auto name = unique_ptr<char, decltype(std::free)*>{
        abi::__cxa_demangle(typeid(T).name(), 0, 0, &status), std::free
    };
    if (status == 0) {
        return name.release();
    } else {
        return typeid(T).name();
    }
}

struct ParseErrorBase: public std::exception {};

template<class T>
struct ParseError: public ParseErrorBase
{
    const char* what() const throw() override
    {
        return (string{"Invalid token: "} + print_type<T>()).c_str();
    }
};

////////////////////////////////////////////////////////////////////////////////

auto x = R"---(

Grammar = (Expression ";")* Expression? Comment? Output

Output =
    | "\"" (OutputStr ComputedValue?)+ "\""
    | EmptyOutput
EmptyOutput = ""
OutputStr = "([^\\$%]*(\\.)?)+"
ComputedValue =
    | PreviousResults
    | PreviousDices
    | GetResult
    | GetDices
GetResult = "$" FullExpression
GetDices = "%" FullExpression

FullExpression = "{" Expression "}"
Expression = Roll Operator+

Comment = ";?#" CommentStr
CommentStr = ".*"

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
Modulo = "%" Quantity
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
// BackwardJump and merge removed (replaced by %x and (%x, %y)
// I don't know what Group means

////////////////////////////////////////////////////////////////////////////////

/** Must match all tokens */
template<class ...Tokens>
struct AllOf { };

/** Return the first matching token */
template<class ...Tokens>
struct OneOf { };

/** Regex */
template<const char* str>
struct R { };

////////////////////////////////////////////////////////////////////////////////

const char number[] = "\\d+";
struct Quantity: OneOf< /* PreviousResults, Not implemented yet */ R<number> > {};

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
const char modulo[] = "%";
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

////////////////////////////////////////////////////////////////////////////////

auto is_valid(auto) {
    return true;
};

template<class ...Tokens>
auto compute(OneOf<Tokens...> tok) {
    for (auto it: tok) {
        if (is_valid(it)) {
            return compute(it);
        }
    }
    throw ParseError("in one of");
}

template<class ...Tokens>
auto compute(AllOf<Tokens...> tokens) {
    for (auto t: tokens) {
        if (!is_valid(t)) {
            throw ParseError("in one of");
        }
    }
    auto ret = string{};
    for (auto t: tokens) {
        ret += compute(t);
    }
    return ret;
}

template<const char* str>
auto compute(R<str>) {
    return str;
}

////////////////////////////////////////////////////////////////////////////////

constexpr auto test_me = "2d3a[=3]*1;4d6;$2*10;($1d10 + %2a[>5])*100;\"$0: $1 [ %1 ]; $2 -> $3 [ %2 -> %3 ]; $4 [ %4 ] -- %0\"";

} // diceparser2

using namespace diceparser2;

int main(int argc, char *argv[])
{
    auto& out = cout;
    auto& err = cerr;

    const auto input = test_me;
    // const auto token = tokenize(input);
    // compute(token);

    // cout << compute(MathOperation{}) << endl;

    return 0;
}

