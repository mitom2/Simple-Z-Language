cmake_minimum_required(VERSION 3.5.0)
project(Simple-Z-Language VERSION 0.1.0 LANGUAGES C CXX)

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
)