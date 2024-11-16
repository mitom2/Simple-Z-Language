#include "CommonFunctions.hpp"

void szl::storeRegisters(std::string &code, szl::Scope &scope)
{
    if (scope.getVariables().count("[REGSAVE]"))
    {
        throw szl::SZLException("Can not store registers more than once in a single scope");
    }
    scope.insertVariable("[REGSAVE]", 12);
    code += "CALL @stdszllib_store_registers\n";
}

void szl::restoreRegisters(std::string &code, szl::Scope &scope)
{
    if (!scope.getVariables().count("[REGSAVE]"))
    {
        throw szl::SZLException("Can not restore registers if they were not stored in the same scope");
    }
    if (scope.getHeadOffset() != scope.getVariables()["[REGSAVE]"].getOffset())
    {
        throw szl::SZLException("Can not restore registers if they are not at the top of the stack");
    }
    int size = scope["[REGSAVE]"].getStackSize();
    scope.getVariables().erase("[REGSAVE]");
    code += "CALL @stdszllib_restore_registers\n";
}
