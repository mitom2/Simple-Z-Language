#include "RuleIdentifier.hpp"

bool szl::RuleIdentifier::check(const szl::Code &content, std::size_t position) const
{
    szl::RuleLiteral literalChk;
    if (literalChk.check(content, position))
        return false;
    szl::RuleKeyword keywordChk;
    if (keywordChk.check(content, position))
        return false;
    if (szl::Rule::isWordCharacter(content[position]) && !szl::Rule::isNumberCharacter(content[position]))
        return true;
    return false;
}

szl::Token szl::RuleIdentifier::generateToken(const szl::Code &content, std::size_t position) const
{
    szl::Token res;
    res.category = szl::TokenCategory::Identifier;
    res.file = content[position].file;
    res.line = content[position].line;
    for (std::size_t i = 0; i + position < content.size(); i++)
    {
        if (!szl::Rule::isWordCharacter(content[position + i]))
        {
            res.content = content.substr(position, i);
            return res;
        }
    }
    if (!res.content.size())
        throw szl::SZLException("Lexical analysis token generation error");
    return res;
}
