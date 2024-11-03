#include "RulePunctuation.hpp"

bool szl::RulePunctuation::check(const std::string &content, std::size_t position) const
{
    return content[position] == ',' || content[position] == '.' || content[position] == ';' || content[position] == ':';
}

szl::Token szl::RulePunctuation::generateToken(const std::string &content, std::size_t position) const
{
    szl::Token res;
    res.category = szl::TokenCategory::Punctuation;
    res.content = content.substr(position, 1);
    return res;
}
