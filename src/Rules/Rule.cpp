#include "Rule.hpp"

bool szl::Rule::isWordCharacter(const char c)
{
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool szl::Rule::isAlphanumericCharacter(const char c)
{
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool szl::Rule::isAlphabeticCharacter(const char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool szl::Rule::isNumberCharacter(const char c)
{
    return c >= '0' && c <= '9';
}

bool szl::Rule::isHexNumberCharacter(const char c)
{
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

bool szl::Rule::isSeparatorCharacter(const char c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\v' || c == '\f' || c == '+' || c == '-' || c == '/' || c == '*' || c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || c == '^' || c == '&' || c == '|' || c == '=' || c == '<' || c == '>' || c == '?' || c == ';' || c == ',';
}