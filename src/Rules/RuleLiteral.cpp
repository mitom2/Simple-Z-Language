#include "RuleLiteral.hpp"

bool szl::RuleLiteral::check(const std::string &content, std::size_t position) const
{
    // Case: character/string
    char sign = content[position];
    if (sign == '\"' || sign == '\'')
    {
        for (std::size_t i = 1; i + position < content.size(); i++)
        {
            if (content[position + i] == sign && content[position + i - 1] != '\\')
                return true;
        }
        return false;
    }

    // Case: true
    if (content.substr(position, 4) == "true" && !szl::Rule::isWordCharacter(content[position + 4]))
        return true;

    // Case: false
    if (content.substr(position, 5) == "false" && !szl::Rule::isWordCharacter(content[position + 5]))
        return true;

    // Case: negative number
    if (content[position] == '-')
    {
        position++;
    }

    // Subcase: decimal number
    if (szl::Rule::isNumberCharacter(content[position]) && (content[position] != '0' || !szl::Rule::isNumberCharacter(content[position + 1])))
    {
        for (std::size_t i = 0; i + position < content.size(); i++)
        {
            if (!szl::Rule::isNumberCharacter(content[position + i]) && content[position + i] != '.')
            {
                if (szl::Rule::isSeparatorCharacter(content[position + i]))
                    return true;
                break;
            }
        }
    }

    // Subcase: non-decimal number
    if (content[position] == '0')
    {
        if (position + 1 < content.size())
        {
            char system = content[position + 1];
            if (system == 'x' || (system >= '1' && system <= '9') || system == 'b')
            {
                for (std::size_t i = 2; i + position < content.size(); i++)
                {
                    if (system == 'x')
                    {
                        if (!szl::Rule::isHexNumberCharacter(content[position + i]) && content[position + i] != '.')
                        {
                            if (szl::Rule::isSeparatorCharacter(content[position + i]))
                                return true;
                            break;
                        }
                    }
                    else if (system == 'b')
                    {
                        if ((content[position + i] != '0' && content[position + i] != '1') && content[position + i] != '.')
                        {
                            if (szl::Rule::isSeparatorCharacter(content[position + i]))
                                return true;
                            break;
                        }
                    }
                    else
                    {
                        if ((content[position + i] < '0' || content[position + i] > '7') && content[position + i] != '.')
                        {
                            if (szl::Rule::isSeparatorCharacter(content[position + i]))
                                return true;
                            break;
                        }
                    }
                }
            }
        }
    }
    return false;
}

szl::Token szl::RuleLiteral::generateToken(const std::string &content, std::size_t position) const
{
    szl::Token res;
    res.category = szl::TokenCategory::Literal;

    // Case: character/string
    char sign = content[position];
    if (sign == '\"' || sign == '\'')
    {
        for (std::size_t i = 1; i + position < content.size(); i++)
        {
            if (content[position + i] == sign && content[position + i - 1] != '\\')
            {
                res.content = content.substr(position, i + 1);
                return res;
            }
        }
    }

    // Case: true
    if (content.substr(position, 4) == "true" && !szl::Rule::isWordCharacter(content[position + 4]))
    {
        res.content = "true";
        return res;
    }

    // Case: false
    if (content.substr(position, 5) == "false" && !szl::Rule::isWordCharacter(content[position + 5]))
    {
        res.content = "false";
        return res;
    }

    // Case: negative number
    if (content[position] == '-')
    {
        res.content = "-";
        position++;
    }
    else
        res.content = "";

    // Subcase: decimal number
    if (szl::Rule::isNumberCharacter(content[position]) && (content[position] != '0' || !szl::Rule::isNumberCharacter(content[position + 1])))
    {
        for (std::size_t i = 0; i + position < content.size(); i++)
        {
            if (!szl::Rule::isNumberCharacter(content[position + i]) && content[position + i] != '.')
            {
                if (szl::Rule::isSeparatorCharacter(content[position + i]))
                {
                    res.content += content.substr(position, i);
                    return res;
                }
                break;
            }
        }
    }

    // Subcase: non-decimal number
    if (content[position] == '0')
    {
        if (position + 1 < content.size())
        {
            char system = content[position + 1];
            if (system == 'x' || (system >= '1' && system <= '9') || system == 'b')
            {
                for (std::size_t i = 2; i + position < content.size(); i++)
                {
                    if (system == 'x')
                    {
                        if (!szl::Rule::isHexNumberCharacter(content[position + i]) && content[position + i] != '.')
                        {
                            if (szl::Rule::isSeparatorCharacter(content[position + i]))
                            {
                                res.content += content.substr(position, i);
                                return res;
                            }
                            break;
                        }
                    }
                    else if (system == 'b')
                    {
                        if ((content[position + i] != '0' && content[position + i] != '1') && content[position + i] != '.')
                        {
                            if (szl::Rule::isSeparatorCharacter(content[position + i]))
                            {
                                res.content += content.substr(position, i);
                                return res;
                            }
                            break;
                        }
                    }
                    else
                    {
                        if ((content[position + i] < '0' || content[position + i] > '7') && content[position + i] != '.')
                        {
                            if (szl::Rule::isSeparatorCharacter(content[position + i]))
                            {
                                res.content += content.substr(position, i);
                                return res;
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
    throw szl::SZLException("Lexical analysis token generation error");
}
