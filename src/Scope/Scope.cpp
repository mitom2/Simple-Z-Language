#include "Scope.hpp"

szl::Scope::Scope(int returnSize, std::string *code, Scope *parent) : code(code), parent(parent), stackHead(nullptr), returnSize(returnSize)
{
    uniqueId = nextUniqueId++;
}

szl::Scope::Scope(int returnSize, Scope *parent) : returnSize(returnSize)
{
    uniqueId = nextUniqueId++;
    code = parent->getCode();
}

szl::Variable szl::Scope::operator[](const std::string &name)
{
    if (!variables.count(name))
    {
        if (parent == nullptr)
            throw szl::SZLException("Variable '" + name + "' not found");
        return parent->operator[](name);
    }
    return variables[name];
}

std::string szl::Scope::operator()(const std::string &name)
{
    if (!variables.count(name))
    {
        if (parent == nullptr)
            throw szl::SZLException("Variable '" + name + "' not found");
        return parent->operator()(name);
    }
    return "LD HL,#" + std::to_string(variables[name].getOffset()) + "\nADD HL,SP\n";
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
    for (auto &it : variables)
    {
        it.second.setOffset(it.second.getNextOffset(var.getStackSize()));
    }
    variables[name] = var;
    stackHead = &variables[name];
}

void szl::Scope::insertVariable(const std::string &name, int size, const std::string &type)
{
    szl::Variable v(size, type);
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

void szl::Scope::renameHead(const std::string &newName)
{
    auto buf = *stackHead;
    for (auto variable = variables.begin(); variable != variables.end(); variable++)
    {
        if ((*variable).second.getOffset() == stackHead->getOffset())
        {
            variable = variables.erase(variable);
            insertVariable(newName, buf.getStackSize(), buf.getType());
            return;
        }
    }
    throw szl::SZLException("Error while assigning variable name '" + newName + "'");
}

void szl::Scope::popHead()
{
    auto size = stackHead->getStackSize();
    szl::Variable *newHead = nullptr;
    for (auto &it : variables)
    {
        if (it.second.getOffset() == 0)
        {
            variables.erase(it.first);
            break;
        }
    }
    for (auto &it : variables)
    {
        it.second.setOffset(it.second.getOffset() - size);
        if (!it.second.getOffset())
            stackHead = &it.second;
    }
}

std::string szl::Scope::getLabel() const
{
    return "@szlCompilerLabelScopeClosedId" + std::to_string(uniqueId);
}

std::string szl::Scope::getNextLabel()
{
    return "@szlCompilerLabelScopeClosedId" + std::to_string(nextUniqueId);
}

void szl::Scope::changeCode(std::string *newCode)
{
    code = newCode;
}

void szl::Scope::addCustomDeleteCode(const std::string &code)
{
    deleteCode = code;
}

szl::Scope::~Scope()
{
    if (!returnSize)
    {
        int offset = 0;
        for (auto variable = variables.begin(); variable != variables.end();)
        {
            offset += (*variable).second.getStackSize();
            variable = variables.erase(variable);
        }
        if (offset)
            *code += "EXX\nLD HL,#" + std::to_string(offset) + "\nADD HL,SP\nLD SP,HL\nEXX\n";
    }
    else if (returnSize > 0)
    {
        int offset = 0;
        for (auto variable = variables.begin(); variable != variables.end();)
        {
            offset += (*variable).second.getStackSize();
            variable = variables.erase(variable);
        }
        if (offset)
            *code += "LD HL,#" + std::to_string(offset) + "\nADD HL,SP\nEX DE,HL\nLD HL,#" + std::to_string(returnSize) + "\nLD B,H\nLD C,L\nADD HL,SP\nLDDR\nEX DE,HL\nLD SP,HL\n";
    }
    *code += deleteCode + "@szlCompilerLabelScopeClosedId" + std::to_string(uniqueId) + "\n";
}
