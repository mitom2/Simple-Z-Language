#include "RuleComment.hpp"

bool szl::RuleComment::check(const std::string &content, std::size_t position) const
{
    if (content.size() <= position + 1)
        return false;
    if (content[position] != '/' || (content[position + 1] != '/' && !content[position + 1] != '*'))
        return false;
    return true;
}

szl::Token szl::RuleComment::generateToken(const std::string &content, std::size_t position) const
{
    szl::Token res;
    res.category = szl::TokenCategory::Comment;
    if (content.size() <= position + 1)
        throw szl::SZLException("Lexical analysis token generation error");
    char closing = content[position + 1];
    for (std::size_t i = 2; i + position + 1 < content.size(); i++)
    {
        if (content[position + i] == closing && content[position + i + 1] == '/')
        {
            res.content = content.substr(position, i + 2);
            return res;
        }
    }
    res.content = content.substr(position, content.size() - position);
    return res;
}
