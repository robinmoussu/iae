namespace iae::grammar {

/* Differences with DiceParser:
 *
 * - Keep and Explode not kept, since 1d10K5 is equivalent to 1d10e[=10]k5
 * - Keep renamed TakeIf => "t"
 * - Sort renamed SortHigher => "S"
 * - added SortLower => "s"
 * - Add renamed AddIf
 * - BackwardJump and merge removed (replaced by !x and (!x, !y)
 * - Group not implemented, I don't know what they are
 */

auto grammar = R"---(

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

}; //namespace iae::grammar
