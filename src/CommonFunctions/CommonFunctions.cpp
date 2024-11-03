#include "CommonFunctions.hpp"

void szl::storeRegisters(szl::Code &code, szl::Scope &scope, bool storeAlternate)
{
    if (scope.getVariables().count("[REGSAVE]"))
    {
        throw szl::SZLException("Can not store registers more than once in a single scope");
    }
    int size = 12 + (storeAlternate ? 8 : 0);
    scope.insertVariable("[REGSAVE]", scope.getNextOffset(size), size);
    if (storeAlternate)
        code.insert("CALL @stdszllib_store_alternate_registers\n");
    else
        code.insert("CALL @stdszllib_store_basic_registers\n");
}

void szl::restoreRegisters(szl::Code &code, szl::Scope &scope)
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

    if (size > 12)
        code.insert("CALL @stdszllib_restore_all_registers\n");
    else
        code.insert("CALL @stdszllib_restore_basic_registers\n");
}
