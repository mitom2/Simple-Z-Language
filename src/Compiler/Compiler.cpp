#include "Compiler.hpp"

szl::Code szl::loadFile(const std::string &path)
{
    std::ifstream file(path);
    if (!file.good())
        throw szl::SZLException("Input file not found");
    std::string input;
    szl::Code content;
    for (std::size_t line = 0; std::getline(file, input); line++)
    {
        content.insert(input, content.size(), path, line);
    }
    file.close();
    return content;
}

std::vector<szl::Token> szl::tokenize(const szl::Code &code)
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
    // TODO: determine heap address
    szl::programData["heap address"] = "$FF00";
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
    std::string filename = in + ".ia", size = "64k", start = "0", output = out;
    std::ofstream file(filename);
    if (!file.good())
        throw szl::SZLException("Output file access denied");
    /** std::ifstream stdszllib("res/stdszllib.ia");
     if (!stdszllib.good())
         throw szl::SZLException("Standard library not found");
     std::string line;
     while (std::getline(stdszllib, line))
     {
         file << line + "\n";
     }
     stdszllib.close();*/
    file << ".include res/stdszllib.ia\n" + res;
    file.close();
    szaman::run(filename, output, size, start, false, false);
}
