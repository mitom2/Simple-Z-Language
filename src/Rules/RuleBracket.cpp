#include "RuleBracket.hpp"

bool szl::RuleBracket::check(const std::string &content, std::size_t position) const
{
    return content[position] == '{' || content[position] == '}' || content[position] == '[' || content[position] == ']' || content[position] == '(' || content[position] == ')' || content[position] == '<' || content[position] == '>';
}

szl::Token szl::RuleBracket::generateToken(const std::string &content, std::size_t position) const
{
    szl::Token res;
    res.category = szl::TokenCategory::Bracket;
    res.content = content.substr(position, 1);
    return res;
}
