#pragma once
#include "Rule.hpp"
#include "SZLException/SZLException.hpp"
#include <unordered_set>

namespace szl
{

    class Rule;

    class RuleKeyword : public szl::Rule
    {
        std::unordered_set<std::string> keywords{
            "+", "-", "/", "*", "%",
            "~", "&", "|", "^", "<<", ">>", "!",
            "=",
            "!=", "==", "<", ">", "<=", ">=",
            "if", "else", "for", "while",
            "int", "uint", "long", "ulong",
            "bool", "float", "char",
            "@", "->", "?",
            "alloc", "free", "memcpy",
            "object",
            "void",
            "in", "out",
            "sizeof",
            "lock();", "unlock();",
            "return"};

    public:
        virtual bool check(const szl::Code &content, std::size_t position) const override;
        virtual szl::Token generateToken(const szl::Code &content, std::size_t position) const override;
    };
} // namespace szl
