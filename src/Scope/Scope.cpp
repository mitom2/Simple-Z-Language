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

bool szl::Scope::exists(const std::string &name) const
{
    if (!variables.count(name))
    {
        if (parent == nullptr)
            return false;
        return parent->exists(name);
    }
    return true;
}

void szl::Scope::insertVariable(const std::string &name, szl::Variable &var)
{
    variables[name] = var;
    stackHead = &variables[name];
}

void szl::Scope::insertVariable(const std::string &name, int size)
{
    insertVariable(name, getNextOffset(size), size);
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
        return parent == nullptr ? nextSize : parent->getNextOffset(nextSize);
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
    int offset = 0;
    for (auto variable = variables.begin(); variable != variables.end();)
    {
        if ((*variable).first == "[REGSAVE]")
        {
            variable++;
            continue;
        }
        offset += (*variable).second.getStackSize();
        variable = variables.erase(variable);
    }
    stackHead = &operator[]("[REGSAVE]");
    *code += "LD HL,#" + std::to_string(offset) + "\nADD HL,SP\nLD SP,HL\n";
    szl::restoreRegisters(*code, *this);
}
