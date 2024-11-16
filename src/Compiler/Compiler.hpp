#pragma once
#include "Code/Code.hpp"
#include "Scope/Scope.hpp"
#include "CommonFunctions/CommonFunctions.hpp"
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
#include <fstream>
#include <iostream>
#include <vector>
#include <list>

namespace szl
{
    std::string loadFile(const std::string &path);

    std::vector<szl::Token> tokenize(const std::string &code);

    void compile(const std::string &in, const std::string &out);
}