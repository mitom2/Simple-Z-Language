#pragma once
#include <string>
#include <vector>
#include <utility>
#include "NextUniqueId/NextUniqueId.hpp"
#include "Tables/Tables.hpp"

namespace szl
{

    class Function
    {
        std::string name;
        std::vector<std::pair<std::string, std::string>> arguments;
        std::string returns;
        std::string label;

    public:
        Function(const std::string &name, const std::vector<std::pair<std::string, std::string>> &arguments, const std::string &returns);

        Function();

        std::string getName() const;

        std::string getLabel() const;

        const std::vector<std::pair<std::string, std::string>> &getArguments() const;

        const std::string &getReturns() const;

        void setName(const std::string &newName);

        void setArguments(const std::vector<std::pair<std::string, std::string>> &newArguments);

        void setReturns(const std::string &newReturns);

        void setLabel(const std::string &newLabel);
    };

} // namespace szl
