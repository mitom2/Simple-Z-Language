#pragma once
#include "Code/Code.hpp"
#include "Scope/Scope.hpp"
#include "SZLException/SZLException.hpp"
#include "Variable/Variable.hpp"

namespace szl
{

    class Scope;

    void storeRegisters(szl::Code &code, szl::Scope &scope, bool storeAlternate = true);

    void restoreRegisters(szl::Code &code, szl::Scope &scope);

} // namespace szl
