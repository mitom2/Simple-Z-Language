#pragma once

namespace szl
{
    class Variable
    {
        int stackSize;
        int offset;

    public:
        Variable(const int offset = 0, const int size = 0);

        int getStackSize() const { return stackSize; }
        int getOffset() const { return offset; }
        int getPosition(const int maxAddr = 0b1111111111111111) const;
        int getNextOffset(const int nextSize) const;
    };
}