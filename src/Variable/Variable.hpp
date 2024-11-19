#pragma once
#include <iostream>

namespace szl
{
    class Variable
    {
        int stackSize;
        int offset;
        std::string type;

    public:
        Variable(const int offset = 0, const int size = 0, const std::string &type = "int");

        int getStackSize() const { return stackSize; }
        int getOffset() const { return offset; }
        int getPosition(const int maxAddr = 0x10000) const;
        int getNextOffset(const int nextSize) const;
        std::string getType() const { return type; }
    };
}