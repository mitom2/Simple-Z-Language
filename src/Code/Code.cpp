#include "Code.hpp"

void szl::Code::insert(CodeLine line, std::size_t position)
{
    code.insert(code.begin() + position, line);
}

szl::CodeLine &szl::Code::operator[](std::size_t position)
{
    return code[position];
}
