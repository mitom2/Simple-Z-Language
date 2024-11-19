#include "Variable.hpp"

szl::Variable::Variable(const int offset, const int size, const std::string &type) : offset(offset), stackSize(size), type(type)
{
}

int szl::Variable::getPosition(const int maxAddr) const
{
    return maxAddr - offset;
}

int szl::Variable::getNextOffset(const int nextSize) const
{
    return offset + nextSize;
}
