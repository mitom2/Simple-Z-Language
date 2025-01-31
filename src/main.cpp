#include <iostream>
#include "Compiler/Compiler.hpp"

int main()
{
    szl::compile("def.szl", "def.oi");
    return 0;
}