#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <utility>
#include "Variable/Variable.hpp"
#include "Tables/Tables.hpp"
#include "SZLException/SZLException.hpp"

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

        std::vector<std::pair<std::string, szl::Variable>> &getVariables() { return variables; }

        void setVariables(const std::vector<std::pair<std::string, szl::Variable>> &variables_) { variables = variables_; }
    };
} // namespace szl
