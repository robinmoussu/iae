#pragma once

#include <any>
#include <type_traits>
#include <memory>
#include <optional>

namespace iae::utility {

/** try to demangle a name using abi::__cxa_demangle */
std::optional<std::string> demangle(const std::string type_name);

template<class T>
std::string print_type()
{
    const auto type_name = typeid(T).name();
    const auto name = demangle(type_name);
    if (name) {
        return std::string{*name};
    } else {
        return typeid(T).name();
    }
}

}; // namespace iae::utility
