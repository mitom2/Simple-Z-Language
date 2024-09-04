#include "Code.hpp"

void szl::Code::insert(CodeLine line, std::size_t position)
{
    code.insert(code.begin() + position, line);
}

void szl::Code::insert(CodeLine line)
{
    insert(line, ++currentPosition);
}

void szl::Code::insert(std::string codeText)
{
    szl::CodeLine codeLine = code[currentPosition];
    codeLine.contents = codeText;
    insert(codeLine, ++currentPosition);
}

szl::CodeLine &szl::Code::operator[](std::size_t position)
{
    return code[position];
}
