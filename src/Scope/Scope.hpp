#pragma once
#include <iostream>
#include <unordered_map>
#include "Variable/Variable.hpp"
#include "SZLException/SZLException.hpp"
#include "CommonFunctions/CommonFunctions.hpp"

namespace szl
{

    class Scope
    {

        Scope *parent;
        std::unordered_map<std::string, szl::Variable> variables;
        szl::Variable *stackHead;
        std::string *code;

    public:
        Scope(std::string *code, Scope *parent = nullptr);

        szl::Variable operator[](const std::string &name);

        szl::Variable *getStackHead() const { return stackHead; }

        std::unordered_map<std::string, szl::Variable> &getVariables() { return variables; }

        Scope *getParent() const { return parent; }

        void insertVariable(const std::string &name, szl::Variable &var);

        void insertVariable(const std::string &name, int offset, int size);

        int getHeadOffset() const;

        int getNextOffset(const int nextSize) const;

        int getStackSize() const;

        ~Scope();
    };

}