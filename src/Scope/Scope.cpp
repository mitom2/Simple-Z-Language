#include "Scope.hpp"

szl::Scope::Scope(std::string *code, Scope *parent) : code(code), parent(parent), stackHead(nullptr)
{
    szl::storeRegisters(*code, *this);
}

szl::Variable szl::Scope::operator[](const std::string &name)
{
    if (!variables.count(name))
    {
        if (parent == nullptr)
            throw szl::SZLException("Variable not found");
        return parent->operator[](name);
    }
    return variables[name];
}

void szl::Scope::insertVariable(const std::string &name, szl::Variable &var)
{
    variables[name] = var;
    stackHead = &variables[name];
}

void szl::Scope::insertVariable(const std::string &name, int offset, int size)
{
    szl::Variable v(offset, size);
    insertVariable(name, v);
}

int szl::Scope::getHeadOffset() const
{
    if (stackHead == nullptr)
        return parent == nullptr ? 0 : parent->getHeadOffset();
    return stackHead->getOffset();
}

int szl::Scope::getNextOffset(const int nextSize) const
{
    if (stackHead == nullptr)
        return parent == nullptr ? 0 : parent->getNextOffset(nextSize);
    return stackHead->getNextOffset(nextSize);
}

int szl::Scope::getStackSize() const
{
    if (stackHead == nullptr)
        return parent == nullptr ? 0 : parent->getStackSize();
    return stackHead->getStackSize();
}

szl::Scope::~Scope()
{
    for (auto variable = variables.begin(); variable != variables.end(); variable++)
    {
        if ((*variable).first == "[REGSAVE]")
            continue;
        for (int i = 0; i < (*variable).second.getStackSize(); i += 2)
        {
            code->insert("POP BC\n");
        }
        variables.erase(variable);
    }
    *code += "LD HL,#" + std::to_string(offset) + "\nADD HL,SP\nLD SP,HL\n";
    szl::restoreRegisters(*code, *this);
}
