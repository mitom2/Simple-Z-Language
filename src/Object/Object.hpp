#pragma once
#include <string>
#include <unordered_map>

namespace szl
{
    class Object
    {
        std::string name;
        std::unordered_map<std::string, std::string> contents;

    public:
        std::string getName() const { return name; }

        void setName(const std::string &name_) { name = name_; }

        const std::unordered_map<std::string, std::string> &getContents() const { return contents; }

        void setAttributes(const std::unordered_map<std::string, std::string> &contents_) { contents = contents_; }
    };
} // namespace szl
