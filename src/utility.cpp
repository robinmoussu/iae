#include "utility.h"

#include <cxxabi.h>

using namespace std;

namespace iae::utility {

optional<string> demangle(const string type_name)
{
    auto status = 0;
    auto name = std::unique_ptr<char, decltype(std::free)*>{
        abi::__cxa_demangle(type_name.c_str(), 0, 0, &status), std::free
    };
    if (status == 0) {
        return name.release();
    } else {
        return {};
    }
}

}; // namespace iae::utility
