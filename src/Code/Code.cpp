#include "Code.hpp"

void szl::Code::insert(std::string codeText, std::size_t position, std::string file, std::size_t line)
{
    for (std::size_t i = 0; i < codeText.size(); i++)
    {
        szl::CodeCharacter newCharacter;
        newCharacter.contents = codeText[i];
        newCharacter.file = file;
        newCharacter.line = line;
        code.insert(code.begin() + position + i, newCharacter);
    }
}

const szl::CodeCharacter &szl::Code::operator[](std::size_t position) const
{
    return code[position];
}

std::size_t szl::Code::size() const
{
    return code.size();
}

std::size_t szl::Code::length() const
{
    return code.size();
}

std::string szl::Code::substr(const std::size_t position, const std::size_t length) const
{
    std::string res;
    for (std::size_t i = 0; i < length && i + position < code.size(); i++)
    {
        res += code[position + i].contents;
    }
    return res;
}

bool szl::CodeCharacter::operator==(const char &ch) const
{
    return contents == ch;
}

szl::CodeCharacter::operator char() const
{
    return contents;
}
