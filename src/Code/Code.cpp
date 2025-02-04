#include "Code.hpp"

void szl::Code::insert(std::string codeText, std::size_t position, std::string file, std::size_t line)
{
    if (!isFileIncluded(file))
        includedFiles.push_back(std::filesystem::absolute(file));
    for (std::size_t i = 0; i < codeText.size(); i++)
    {
        szl::CodeCharacter newCharacter;
        newCharacter.contents = codeText[i];
        newCharacter.file = file;
        newCharacter.line = std::to_string(line);
        code.insert(code.begin() + position + i, newCharacter);
    }
}

void szl::Code::insert(const szl::Code &codeText, std::size_t position)
{
    for (std::size_t i = 0; i < codeText.size(); i++)
    {
        szl::CodeCharacter newCharacter = codeText[i];
        code.insert(code.begin() + position + i, newCharacter);
        if (!isFileIncluded(newCharacter.file))
            includedFiles.push_back(std::filesystem::absolute(newCharacter.file));
    }
}

void szl::Code::insert(const szl::CodeCharacter &codeChar, std::size_t position)
{
    szl::CodeCharacter newCharacter = codeChar;
    code.insert(code.begin() + position, newCharacter);
    if (!isFileIncluded(newCharacter.file))
        includedFiles.push_back(std::filesystem::absolute(newCharacter.file));
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

bool szl::Code::isFileIncluded(const std::string &file) const
{
    for (auto &it : includedFiles)
    {
        if (std::filesystem::weakly_canonical(std::filesystem::absolute(it)) == std::filesystem::weakly_canonical(std::filesystem::absolute(file)))
            return true;
    }
    return false;
}

bool szl::CodeCharacter::operator==(const char &ch) const
{
    return contents == ch;
}

szl::CodeCharacter::operator char() const
{
    return contents;
}
