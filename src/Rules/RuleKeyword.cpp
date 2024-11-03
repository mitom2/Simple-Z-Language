#include "RuleKeyword.hpp"

bool szl::RuleKeyword::check(const std::string &content, std::size_t position) const
{
    if (content[position] == '/')
    {
        if (position + 1 < content.length())
        {
            if (content[position + 1] == '/' || content[position + 1] == '*')
                return false;
        }
    }
    int max = 0;
    for (const auto &it : keywords)
    {
        max = max > it.length() ? max : it.length();
    }
    for (std::size_t i = 1; i + position < content.size(); i++)
    {
        if (keywords.count(content.substr(position, i)) != 0)
            return true;
        if (i > max)
            return false;
    }
    return false;
}

szl::Token szl::RuleKeyword::generateToken(const std::string &content, std::size_t position) const
{
    if (content[position] == '/')
    {
        if (position + 1 < content.length())
        {
            if (content[position + 1] == '/' || content[position + 1] == '*')
                throw szl::SZLException("Lexical analysis token generation error");
        }
    }
    szl::Token res;
    res.category = szl::TokenCategory::Keyword;
    int max = 0;
    for (const auto &it : keywords)
    {
        max = max > it.length() ? max : it.length();
    }
    std::unordered_set<std::string> matches;
    for (std::size_t i = 1; i + position < content.size(); i++)
    {
        if (keywords.count(content.substr(position, i)))
        {
            matches.insert(content.substr(position, i));
        }
        if (i > max)
            break;
    }
    std::string maxMatch;
    for (const auto &it : matches)
    {
        maxMatch = maxMatch.length() > it.length() ? maxMatch : it;
    }
    if (!maxMatch.length())
        throw szl::SZLException("Lexical analysis token generation error");
    res.content = maxMatch;
    return res;
}
