#include "parser.h"
#include "utility.h"

#include <string>

using namespace iae::utility;
using namespace std;

namespace iae::grammar {

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
