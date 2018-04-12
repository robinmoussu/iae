#include <iostream> // TODO fmt
#include <string>
#include <exception>
#include <regex> // TODO boost
//#include <string_view>

using namespace std;

////////////////////////////////////////////////////////////////////////////////

class ParseError: public std::exception 
{
private:
	const std::string description_;
public:
	ParseError(std::string description) : description_(description) {}
	const char* what() const throw() override
	{
		return description_.c_str();
	}
};

////////////////////////////////////////////////////////////////////////////////

class Token {};

class Expression: public Token, private vector<Token> {
public:
    void push_back (const Token& val) {
        vector::push_back(val);
    }
};

class Number: public Token {
private:
    std::string tok;
public:
    Number(const std::string& tok)
        : tok(tok) {
    }
};

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
// Don't know what Group means


////////////////////////////////////////////////////////////////////////////////

// TODO std::basic_string_view
Token tokenize(const std::string& in)
{
    auto ret = Expression{};

    auto it = in.cbegin();
    const auto end = in.cend();

    do {
        //smatch m;
        //if(regex_match(it, end, m, regex("\\d\\+"))) {
            //ret.push_back(Number(m[0]));
        if(*it == '2') {
            ret.push_back(Number("2"));
            it += 1;
        } else {
            throw ParseError(string(it, end));
        }
    }while(it != end);

	return ret;
}

void compute(const Token& in)
{
}

////////////////////////////////////////////////////////////////////////////////

const auto test_me = string("2d3a[=3]*1;4d6;$2*10;($1d10 + %2a[>5])*100;\"$0: $1 [ %1 ]; $2 -> $3 [ %2 -> %3 ]; $4 [ %4 ] -- %0\"");
int main(int argc, char *argv[])
{
	auto& out = cout;
	auto& err = cerr;

	const auto input = test_me;
    const auto token = tokenize(input);
    compute(token);

	return 0;
}
