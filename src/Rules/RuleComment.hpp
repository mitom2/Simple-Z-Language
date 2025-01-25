#pragma once
#include "Rule.hpp"
#include "SZLException/SZLException.hpp"

namespace szl
{
    class RuleComment : public szl::Rule
    {
    public:
        virtual bool check(const szl::Code &content, std::size_t position) const override;
        virtual szl::Token generateToken(const szl::Code &content, std::size_t position) const override;
    };
}