#include "Variable.hpp"

szl::Variable::Variable(int offset, int size) : offset(offset), stackSize(size)
{
}

int szl::Variable::getPosition(int maxAddr) const
{
    return maxAddr - offset;
}

int szl::Variable::getNextOffset(int nextSize) const
{
    return offset + nextSize;
}
