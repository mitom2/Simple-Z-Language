#pragma once
#include "Code/Code.hpp"
#include "Scope/Scope.hpp"
#include "SZLException/SZLException.hpp"
#include "Variable/Variable.hpp"
#include <iostream>

namespace szl
{

    class Scope;

    void storeRegisters(std::string &code, szl::Scope &scope);

    void restoreRegisters(std::string &code, szl::Scope &scope);

} // namespace szl
