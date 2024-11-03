#pragma once
#include "Rule.hpp"

namespace szl
{

    class Rule;

    class RulePunctuation : public szl::Rule
    {
    public:
        virtual bool check(const std::string &content, std::size_t position) const override;
        virtual szl::Token generateToken(const std::string &content, std::size_t position) const override;
    };
} // namespace szl
