#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <utility>
#include "Variable/Variable.hpp"

namespace szl
{
    class Object
    {
        std::string name;
        std::unordered_map<std::string, std::string> contents;
        std::vector<std::pair<std::string, szl::Variable>> variables;

    public:
        std::string getName() const { return name; }

        void setName(const std::string &name_) { name = name_; }

        std::unordered_map<std::string, std::string> &getContents() { return contents; }

        void setContents(const std::unordered_map<std::string, std::string> &contents_) { contents = contents_; }

        int getSize() const;

        int getVariablePosition(const int objectPosition, const std::string &name) const;
    };
} // namespace szl
