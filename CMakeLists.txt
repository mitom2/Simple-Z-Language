cmake_minimum_required(VERSION 3.12)
project(Simple-Z-Language VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src/)
add_library(SZAMAN INTERFACE)
target_include_directories(SZAMAN INTERFACE inc/SZAMAN)

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
target_link_libraries(Simple-Z-Language PRIVATE SZAMAN)
add_custom_command(TARGET Simple-Z-Language POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/res
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/res
    ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/res
)
add_custom_command(TARGET Simple-Z-Language POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/examples
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/examples
    ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/examples
)