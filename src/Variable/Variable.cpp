#include "Variable.hpp"

szl::Variable::Variable(const int size, const std::string &type) : offset(size - 1), stackSize(size), type(type)
{
}

int szl::Variable::getNextOffset(const int nextSize) const
{
    return offset + nextSize;
}
