#include "RuleWhitespace.hpp"

bool szl::RuleWhitespace::check(const std::string &content, std::size_t position) const
{
    for (std::size_t i = 0; i < 6; i++)
    {
        if (content[position] == whitespaces[i])
        {
            return true;
        }
    }
    return false;
}

szl::Token szl::RuleWhitespace::generateToken(const std::string &content, std::size_t position) const
{
    szl::Token res;
    res.category = TokenCategory::Whitespace;
    for (std::size_t i = 0; i + position < content.size(); i++)
    {
        bool found = false;
        for (std::size_t o = 0; o < 6; o++)
        {
            if (whitespaces[o] == content[i + position])
                found = true;
        }
        if (!found)
        {
            res.content = content.substr(position, i);
            return res;
        }
    }
    res.content = content.substr(position, content.size() - position);
    return res;
}
