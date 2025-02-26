#pragma once
#include "Rule.hpp"
#include "SZLException/SZLException.hpp"
#include "Rules/RuleKeyword.hpp"
#include "Rules/RuleLiteral.hpp"

namespace szl
{

    class Rule;

    class RuleIdentifier : public szl::Rule
    {
    public:
        virtual bool check(const szl::Code &content, std::size_t position) const override;
        virtual szl::Token generateToken(const szl::Code &content, std::size_t position) const override;
    };
} // namespace szl
