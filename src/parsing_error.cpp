#include "parsing_error.h"
#include "utility.h"

#include <string>

using namespace iae::utility;
using namespace std;

namespace iae::grammar {

string to_string(const ParseError& e) {
    return e.to_string();
}

ParseError::ParseError(const ParseError::ErrorInfoBase* e)
    : error_info{e}
{ }
string ParseError::to_string() const {
    return error_info->to_string();
}

ParseError::ErrorInfoBase::~ErrorInfoBase() = default;

}; // namespace iae::grammar
