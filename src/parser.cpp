#include "parser.h"
#include "utility.h"

#include <string>

using namespace iae::utility;
using namespace std;

namespace iae::grammar {

string to_string(output_t o) {
    return visit(overloaded {
        [](error_t e) {
            return to_string(e);
        },
        [](success_t s) {
            return string{s};
        },
    }, o);
}

}; // namespace iae::grammar
