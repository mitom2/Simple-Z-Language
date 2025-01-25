#include "SZLException.hpp"

const char *szl::SZLException::what() const noexcept
{
    if (!file.length() || !line.length())
        return ("[Unable to determine error location in code]: " + msg).c_str();
    return ("[" + file + ": " + line + "]: " + msg).c_str();
}
