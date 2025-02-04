#include "SZLException.hpp"

std::string szl::SZLException::wht() const noexcept
{
    if (!file.length() || !line.length())
        return "[Unable to determine error location in code]: " + msg;
    return "[" + file + ": " + line + "]: " + msg;
}
