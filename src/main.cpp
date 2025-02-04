#include <iostream>
#include "Compiler/Compiler.hpp"
#include "SZLException/SZLException.hpp"

int main(int argc, char **argv)
{
    std::string in, out;
    for (int i = 0; i + 1 < argc; i++)
    {
        if (std::string(argv[i]) == "-i")
            in = argv[i + 1];
        else if (std::string(argv[i]) == "-o")
            out = argv[i + 1];
    }
    if (!in.length() || !out.length())
    {
        std::cout << "Too few arguments. Usage: szl -i [input file] -o [output file]\n";
        return 64;
    }
    try
    {
        szl::compile(in, out);
    }
    catch (szl::SZLException &e)
    {
        std::cout << e.wht() << "\nProgram was not compiled.\n";
        return 65;
    }
    return 0;
}
