#include "Compiler.hpp"

std::string szl::loadFile(const std::string &path)
{
    std::ifstream file(path);
    if (!file.good())
        throw szl::SZLException("Input file not found");
    std::string input, content;
    while (file >> input)
    {
        content += input + " ";
    }
    file.close();
    return content;
}

std::vector<szl::Token> szl::tokenize(const std::string &code)
{
    std::list<szl::Rule *> allRules = {new szl::RuleWhitespace, new szl::RuleComment, new szl::RuleLiteral, new szl::RuleBracket, new szl::RulePunctuation, new szl::RuleKeyword, new szl::RuleIdentifier};
    std::list<szl::Rule *> rules;
    std::vector<szl::Token> res;
    bool comment = false;
    for (std::size_t i = 0; i < code.size();)
    {
        rules = allRules;
        for (auto it = rules.begin(); it != rules.end();)
        {
            if (!(*it)->check(code, i))
                it = rules.erase(it);
            else
                it++;
        }
        if (rules.size() == 1)
        {
            auto token = (*(rules.begin()))->generateToken(code, i);
            if (token.category != szl::TokenCategory::Comment && token.category != szl::TokenCategory::Whitespace)
                res.push_back(token);
            i += token.content.length();
            continue;
        }
        else
            throw szl::SZLException("Lexical error");
    }
    for (auto &it : allRules)
    {
        delete it;
    }
    return res;
}

void szl::compile(const std::string &in, const std::string &out)
{
    auto program = tokenize(loadFile(in));
    szl::Grammar grammar;
    grammar.initialize();
    // TODO: set program name using preprocessor
    szl::programData["name"] = "program";
    std::string res;
    {
        std::list<szl::Scope> scopes;
        scopes.emplace_back(12, &res);
        for (std::size_t i = 0; i < program.size();)
        {
            std::vector<std::string> internal;
            auto code = grammar.execute(program, i, scopes, internal);
            if (code != "")
            {
                res += code;
                continue;
            }
            throw szl::SZLException("Syntax error");
        }
    }
    std::cout << res << " " << std::endl;
}
