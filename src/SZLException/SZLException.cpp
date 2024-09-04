#include "SZLException.hpp"

const char *szl::SZLException::what() const noexcept
{
    return msg.c_str();
}
