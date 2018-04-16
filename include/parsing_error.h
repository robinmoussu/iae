#pragma once

#include <any>
#include <string>

namespace iae::grammar {

class ParseError {
    class ErrorInfoBase;
    ParseError() = delete;

public:
    ParseError(const ErrorInfoBase*);
    ParseError(ParseError&&) = default;
    ParseError(const ParseError&) = default;

    template <class T>
    static ParseError create();

    std::string to_string() const;

private:
    const ErrorInfoBase* error_info;

private:
    class ErrorInfoBase {
    public:
        virtual std::string to_string() const = 0;
        virtual ~ErrorInfoBase() = 0;
    };

    template <class T>
    class ErrorInfo: public ErrorInfoBase {
    public:
        std::string to_string() const override;
        ~ErrorInfo() override;
    public:
        static const ErrorInfo<T> instance_v;
    };
};

/**
 * Pretty print a parsing error
 */
std::string to_string(const ParseError&);

}; // namespace iae::grammar

#include "implementation/parsing_error.cxx"
