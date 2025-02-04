#pragma once
#include <exception>
#include <string>

namespace szl
{
    class SZLException : public std::exception
    {
        std::string msg;
        std::string line;
        std::string file;

    public:
        SZLException(std::string msg, const std::string &file, const std::string &line) : msg(msg), file(file), line(line) {}
        SZLException(std::string msg) : msg(msg), file(""), line("") {}
        std::string wht() const noexcept;
    };
} // namespace szl
