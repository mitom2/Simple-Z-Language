#pragma once

namespace szl
{
    class Variable
    {
        int stackSize;
        int offset;

    public:
        Variable(int offset, int size);

        int getStackSize() const { return stackSize; }
        int getOffset() const { return offset; }
        int getPosition(int maxAddr = 0b1111111111111111) const;
        int getNextOffset(int nextSize) const;
    };
}