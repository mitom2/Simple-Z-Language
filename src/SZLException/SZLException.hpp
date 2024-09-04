#pragma once
#include <exception>
#include <string>

namespace szl
{
    class SZLException : public std::exception
    {
        std::string msg;

    public:
        SZLException(std::string msg) : msg(msg) {}
        virtual const char *what() const noexcept;
    };
} // namespace szl
