#include <iostream>
#include "Compiler/Compiler.hpp"

int main()
{
    szl::compile("def.szl", "def.ia");
    return 0;
}