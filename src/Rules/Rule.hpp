#pragma once
#include <iostream>
#include "Code/Code.hpp"

namespace szl
{
    enum TokenCategory
    {
        Identifier,
        Keyword,
        Punctuation,
        Bracket,
        Literal,
        Comment,
        Whitespace
    };

    struct Token
    {
        TokenCategory category;
        std::string content;
        std::string file;
        std::string line;
    };

    class Rule
    {
    protected:
        static bool isWordCharacter(const char c);
        static bool isAlphanumericCharacter(const char c);
        static bool isAlphabeticCharacter(const char c);
        static bool isNumberCharacter(const char c);
        static bool isHexNumberCharacter(const char c);
        static bool isSeparatorCharacter(const char c);

    public:
        virtual bool check(const szl::Code &content, std::size_t position) const = 0;
        virtual szl::Token generateToken(const szl::Code &content, std::size_t position) const = 0;
    };
}