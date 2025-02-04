#pragma once
#include "Code/Code.hpp"
#include "Scope/Scope.hpp"
#include "SZLException/SZLException.hpp"
#include "Rules/Rule.hpp"
#include "Rules/RuleWhitespace.hpp"
#include "Rules/RuleComment.hpp"
#include "Rules/RuleLiteral.hpp"
#include "Rules/RuleBracket.hpp"
#include "Rules/RulePunctuation.hpp"
#include "Rules/RuleKeyword.hpp"
#include "Rules/RuleIdentifier.hpp"
#include "Grammar/Grammar.hpp"
#include "SZAMAN.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <list>
#include <filesystem>

namespace szl
{
    szl::Code loadFile(const std::string &path, const std::string &local);

    std::vector<szl::Token> tokenize(const szl::Code &code);

    szl::Code preprocessor(szl::Code code, const std::string &localPath);

    void compile(const std::string &in, const std::string &out);
}