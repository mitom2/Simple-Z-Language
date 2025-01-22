cmake_minimum_required(VERSION 3.5.0)
project(Simple-Z-Language VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

include_directories(src/)

add_executable(Simple-Z-Language 
src/main.cpp
src/Scope/Scope.cpp
src/Variable/Variable.cpp
src/Code/Code.cpp
src/SZLException/SZLException.cpp
src/CommonFunctions/CommonFunctions.cpp
src/Compiler/Compiler.cpp
src/Rules/Rule.cpp
src/Rules/RuleWhitespace.cpp
src/Rules/RuleComment.cpp
src/Rules/RuleLiteral.cpp
src/Rules/RuleBracket.cpp
src/Rules/RulePunctuation.cpp
src/Rules/RuleKeyword.cpp
src/Rules/RuleIdentifier.cpp
src/Grammar/Grammar.cpp
src/NextUniqueId/NextUniqueId.cpp
src/Tables/Tables.cpp
src/Function/Function.cpp
src/Object/Object.cpp
)