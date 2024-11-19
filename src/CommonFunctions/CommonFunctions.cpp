#include "CommonFunctions.hpp"

void szl::storeRegisters(int returnSize, std::string &code, szl::Scope &scope)
{
    if (scope.getVariables().count("[REGSAVE]"))
    {
        throw szl::SZLException("Can not store registers more than once in a single scope");
    }
    if (returnSize == 0)
    {
        scope.insertVariable("[REGSAVE]", 12, "[REGSAVE]");
        code += "CALL @stdszllib_store_all_registers\n";
    }
    else if (returnSize == 2)
    {
        scope.insertVariable("[REGSAVE]", 10, "[REGSAVE]");
        code += "CALL @stdszllib_store_default_registers\n";
    }
    else if (returnSize == 4)
    {
        scope.insertVariable("[REGSAVE]", 8, "[REGSAVE]");
        code += "CALL @stdszllib_store_basic_registers\n";
    }
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
    if (size == 8)
        code += "CALL @stdszllib_restore_basic_registers\n";
    if (size == 10)
        code += "CALL @stdszllib_restore_default_registers\n";
    if (size == 12)
        code += "CALL @stdszllib_restore_all_registers\n";
}
