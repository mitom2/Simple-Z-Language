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
            "+", "-", "/", "*",
            "++", "--",
            "~", "&", "|", "^", "<<", ">>",
            "!", "&&", "||", "^^",
            "=", "+=", "-=", "*=", "/=", "~=", "&=", "|=", "^=",
            "!=", "==", "<", ">", "<=", ">=",
            "int", "uint", "int16", "uint16", "int32", "uint32",
            "bool", "float", "char", "string", "array", "list", "map",
            "struct", "class",
            "if", "for", "while", "?", "?:",
            "inline", "void", "static",
            "in", "out",
            "::",
            "return"};

    public:
        virtual bool check(const std::string &content, std::size_t position) const override;
        virtual szl::Token generateToken(const std::string &content, std::size_t position) const override;
    };
} // namespace szl
