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

        std::unordered_map<std::string, std::string> &getContents() { return contents; }

        void setContents(const std::unordered_map<std::string, std::string> &contents_) { contents = contents_; }

        int getSize() const;
    };
} // namespace szl
