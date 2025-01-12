#include "Scope.hpp"

szl::Scope::Scope(int returnSize, std::string *code, Scope *parent) : code(code), parent(parent), stackHead(nullptr)
{
    uniqueId = nextUniqueId++;
    if (returnSize >= 0)
    {
        szl::storeRegisters(returnSize, *code, *this);
        skipCleanup = false;
    }
    else
        skipCleanup = true;
}

szl::Scope::Scope(int returnSize, Scope *parent)
{
    uniqueId = nextUniqueId++;
    code = parent->getCode();
    if (returnSize >= 0)
    {
        szl::storeRegisters(returnSize, *code, *this);
        skipCleanup = false;
    }
    else
        skipCleanup = true;
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

void szl::Scope::insertVariable(const std::string &name, int size, const std::string &type)
{
    insertVariable(name, getNextOffset(size), size, type);
}

void szl::Scope::insertVariable(const std::string &name, int offset, int size, const std::string &type)
{
    szl::Variable v(offset, size, type);
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
    throw szl::SZLException("Error while assigning variable name");
}

void szl::Scope::popHead()
{
    auto offset = stackHead->getOffset();
    szl::Variable *newHead = nullptr;
    for (auto &it : variables)
    {
        if (it.second.getOffset() == offset)
        {
            variables.erase(it.first);
            break;
        }
    }
    for (auto &it : variables)
    {
        if (newHead != nullptr)
        {
            newHead = newHead->getOffset() < it.second.getOffset() ? &(it.second) : newHead;
        }
    }
    stackHead = newHead;
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
    if (!skipCleanup)
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
        if (!variables.count("[REGSAVE]"))
            throw szl::SZLException("Registers restoration failed while concluding scope");
        stackHead = &variables["[REGSAVE]"];
        if (offset)
            *code += "EXX\nLD HL,#" + std::to_string(offset) + "\nADD HL,SP\nLD SP,HL\nEXX\n";
        szl::restoreRegisters(*code, *this);
    }
    *code += deleteCode + "@szlCompilerLabelScopeClosedId" + std::to_string(uniqueId) + "\n";
}
