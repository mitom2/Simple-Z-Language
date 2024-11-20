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
        bool skipCleanup;

    public:
        Scope(int returnSize, std::string *code, Scope *parent = nullptr);

        Scope(int returnSize, Scope *parent);

        szl::Variable operator[](const std::string &name);

        bool exists(const std::string &name) const;

        szl::Variable *getStackHead() const { return stackHead; }

        std::unordered_map<std::string, szl::Variable> &getVariables() { return variables; }

        Scope *getParent() const { return parent; }

        void insertVariable(const std::string &name, szl::Variable &var);

        void insertVariable(const std::string &name, int size, const std::string &type);

        void insertVariable(const std::string &name, int offset, int size, const std::string &type);

        int getHeadOffset() const;

        int getNextOffset(const int nextSize) const;

        int getStackSize() const;

        void renameHead(const std::string &newName);

        std::string *getCode() { return code; };

        void popHead();

        ~Scope();
    };

}