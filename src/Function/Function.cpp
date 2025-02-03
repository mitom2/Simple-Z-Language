#include "Function/Function.hpp"
#include "Function.hpp"

szl::Function::Function(const std::string &name, const std::vector<std::pair<std::string, std::string>> &arguments, const std::string &returns) : name(name), arguments(arguments), returns(returns), label("@szlCompilerUserFunction" + name + "Id" + std::to_string(szl::nextUniqueId++) + "\n") {}

szl::Function::Function() : name("newfn"), arguments({}), returns("void"), label("@szlCompilerNewEmptyFn") {}

std::string szl::Function::getName() const
{
    return name;
}

std::string szl::Function::getLabel() const
{
    return name == "main" ? "@entry_point_" + szl::programData["name"] + "\nCALL @stdszllib_heap_init\n" : label;
}

const std::vector<std::pair<std::string, std::string>> &szl::Function::getArguments() const
{
    return arguments;
}

const std::string &szl::Function::getReturns() const
{
    return returns;
}

void szl::Function::setName(const std::string &newName)
{
    name = newName;
}

void szl::Function::setArguments(const std::vector<std::pair<std::string, std::string>> &newArguments)
{
    arguments = newArguments;
}

void szl::Function::setReturns(const std::string &newReturns)
{
    returns = newReturns;
}

void szl::Function::setLabel(const std::string &newLabel)
{
    label = newLabel;
}
