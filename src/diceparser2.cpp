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
