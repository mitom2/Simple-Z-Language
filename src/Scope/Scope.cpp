#include "Scope.hpp"

szl::Scope::Scope(Code *code, Scope *parent) : code(code), parent(parent)
{
    szl::storeRegisters(*code, *this);
}

szl::Variable szl::Scope::findVar(const std::string &name)
{
    if (!variables.count(name))
    {
        if (parent == nullptr)
            throw szl::SZLException("Variable not found");
        return parent->findVar(name);
    }
    return variables[name];
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
    szl::restoreRegisters(*code, *this);
}
