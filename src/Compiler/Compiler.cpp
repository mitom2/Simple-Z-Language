#include "Compiler.hpp"

szl::Code szl::loadFile(const std::string &path)
{
    std::ifstream file(path);
    if (!file.good())
        throw szl::SZLException("Input file '" + path + "' not found");
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

szl::Code szl::preprocessor(szl::Code code)
{
    szl::Code res;
    for (std::size_t i = 0; i < code.size(); i++)
    {
        if (code[i] == '#')
        {
            std::string directive = code.substr(i, 8);
            if (directive == "#include")
            {
                i += 8;
                if (code.size() <= i)
                    throw szl::SZLException("Unexpected EOF on preprocessor directive #include", code[i].file, code[i].line);
                std::string path;
                bool found = false;
                for (; i < code.size(); i++)
                {
                    if (code[i] == '"')
                    {
                        if (found)
                            break;
                        found = true;
                        continue;
                    }
                    if (!found)
                        continue;
                    path += code[i];
                }
                if (!code.isFileIncluded(path))
                    code.insert(loadFile(path), i + 1);
            }
            else if (directive == "#program")
            {
                i += 8;
                if (code.size() <= i)
                    throw szl::SZLException("Unexpected EOF on preprocessor directive #program", code[i].file, code[i].line);
                std::string name;
                bool found = false;
                for (; i < code.size(); i++)
                {
                    if (code[i] == '"')
                    {
                        if (found)
                            break;
                        found = true;
                        continue;
                    }
                    if (!found)
                        continue;
                    if (code[i] == ' ' || code[i] == '\n' || code[i] == '\t')
                        throw szl::SZLException("Program name can not contain white characters", code[i].file, code[i].line);
                    name += code[i];
                }
                if (!name.length())
                    throw szl::SZLException("Preprocessor directive #program requires string containing identifier at least 1 valid character long", code[i].file, code[i].line);
                szl::programData["name"] = name;
            }
            else if ((directive = code.substr(i, 9)) == "#position")
            {
                i += 9;
                if (code.size() <= i)
                    throw szl::SZLException("Unexpected EOF on preprocessor directive #position", code[i].file, code[i].line);
                std::string position;
                bool found = false;
                for (; i < code.size(); i++)
                {
                    if (code[i] == '"')
                    {
                        if (found)
                            break;
                        found = true;
                        continue;
                    }
                    if (!found)
                        continue;
                    position += code[i];
                }
                if (!position.length())
                    throw szl::SZLException("Preprocessor directive #position requires string containing non-negative decimal number", code[i].file, code[i].line);
                for (std::size_t i = 0; i < position.length(); i++)
                {
                    if (position[i] < '0' || position[i] > '9')
                        throw szl::SZLException("Preprocessor directive #position requires string containing non-negative decimal number", code[i].file, code[i].line);
                }
                if (std::stol(position) > 0xFFFF)
                    throw szl::SZLException("Preprocessor directive #position requires string containing number lower than 65536", code[i].file, code[i].line);
                szl::programData["position"] = position;
            }
            else
                throw szl::SZLException("Preprocessor directive not recognized", code[i].file, code[i].line);
        }
        else
        {
            res.insert(code[i], res.size());
        }
    }
    return res;
}

void szl::compile(const std::string &in, const std::string &out)
{
    auto program = tokenize(preprocessor(loadFile(in)));
    if (!szl::programData.count("name"))
        throw szl::SZLException("Preprocessor directive #program not found");
    szl::programData["position"] = szl::programData.count("position") ? szl::programData["position"] : "16384"; // Set HEX 4000 if not specified
    szl::Grammar grammar;
    grammar.initialize();
    std::string res;
    {
        std::list<szl::Scope> scopes;
        scopes.emplace_back(-1, &res);
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
    res = ".include res/stdszllib.ia\n" + res + "\n@szlCompilerHeapPosition\n";
    std::string filename = in + ".ia", size = "64k", output = out;
    std::ofstream file(filename);
    if (!file.good())
        throw szl::SZLException("Output file access denied");
    file << res;
    file.close();
    szaman::run(filename, output, size, szl::programData["position"], false, false);
}
