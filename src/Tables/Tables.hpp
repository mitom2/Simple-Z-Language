#pragma once
#include <unordered_map>
#include <string>
#include "Function/Function.hpp"
#include "Object/Object.hpp"

namespace szl
{
    class Function;
    extern std::unordered_map<std::string, szl::Function> functions;
    extern std::unordered_map<std::string, std::string> programData;
    extern std::unordered_map<std::string, szl::Object> objectTypes;
} // namespace szl
