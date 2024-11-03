#pragma once
#include "Rule.hpp"

namespace szl
{

    class Rule;

    class RuleWhitespace : public szl::Rule
    {
        char whitespaces[6] = {' ', '\t', '\r', '\n', '\v', '\f'};

    public:
        virtual bool check(const std::string &content, std::size_t position) const override;
        virtual szl::Token generateToken(const std::string &content, std::size_t position) const override;
    };
} // namespace szl
