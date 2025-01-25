#include "RuleBracket.hpp"

bool szl::RuleBracket::check(const szl::Code &content, std::size_t position) const
{
    return content[position] == '{' || content[position] == '}' || content[position] == '[' || content[position] == ']' || content[position] == '(' || content[position] == ')';
}

szl::Token szl::RuleBracket::generateToken(const szl::Code &content, std::size_t position) const
{
    szl::Token res;
    res.category = szl::TokenCategory::Bracket;
    res.content = content.substr(position, 1);
    res.file = content[position].file;
    res.line = content[position].line;
    return res;
}
