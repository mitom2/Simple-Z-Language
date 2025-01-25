#pragma once
#include "Rule.hpp"

namespace szl
{

    class Rule;

    class RuleBracket : public szl::Rule
    {
    public:
        virtual bool check(const szl::Code &content, std::size_t position) const override;
        virtual szl::Token generateToken(const szl::Code &content, std::size_t position) const override;
    };
} // namespace szl
