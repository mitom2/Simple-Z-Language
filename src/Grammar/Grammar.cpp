#include "Grammar.hpp"

std::string szl::Grammar::toBin(uint32_t in)
{
    std::string res;
    for (; in > 0; in /= 2)
    {
        if (in % 2)
            res = "1" + res;
        else
            res = "0" + res;
    }
    if (res.length() < 1)
        res = "0";
    return res;
}

std::string szl::Grammar::toBinFloat(double in)
{
    std::string rRes;
    long lPart = static_cast<long>(in);
    std::string lRes = szl::Grammar::toBin(lPart);
    double rPart = (in - (double)lPart);
    for (int prec = 0; (rPart != 0.0) && (prec < 23); prec++)
    {
        rPart *= 2.0;
        if (rPart >= 1.0)
        {
            rPart -= 1.0;
            rRes += "1";
        }
        else
            rRes += "0";
    }
    int shift = 0;
    if (lRes.length() > 1)
    {
        shift -= lRes.length() - 1;
    }
    else if (lPart == 0)
    {
        for (std::size_t i = 0; i < rRes.size(); i++)
        {
            if (rRes[i] == '1')
            {
                shift = i + 1;
                break;
            }
        }
    }
    if (shift < 0)
    {
        rRes = lRes.substr(1, lRes.length() - 1) + rRes;
        lRes = lRes.substr(0, 1);
    }
    else if (shift > 0)
    {
        lRes = rRes.substr(shift - 1, shift);
        rRes = rRes.substr(shift, rRes.length() - shift);
    }
    shift = 127 - shift;
    if (shift < 0)
        throw szl::SZLException("Error on float conversion: exponent too small");
    std::string exponent = szl::Grammar::toBin(shift);
    if (exponent.length() > 8)
        throw szl::SZLException("Error on float conversion: exponent too big");
    while (exponent.length() < 8)
    {
        exponent = "0" + exponent;
    }
    while (rRes.length() < 23)
    {
        rRes += "0";
    }
    return "0" + exponent + rRes;
}

std::string szl::Grammar::fromOct(const std::string &in)
{
    std::string buf = in.substr(1, in.length() - 1);
    long res = 0;
    for (std::size_t i = 0; i < buf.length(); i++)
    {
        res += (buf[i] - '0') * std::pow(8, buf.length() - i - 1);
    }
    return szl::Grammar::toBin(res);
}

std::string szl::Grammar::fromHex(const std::string &in)
{
    std::string buf = in.substr(2, in.length() - 2);
    uint32_t res = 0;
    for (std::size_t i = 0; i < buf.length(); i++)
    {
        if (buf[i] <= '9')
            res += (buf[i] - '0') * std::pow(16, buf.length() - i - 1);
        else if (buf[i] <= 'Z')
        {
            res += (buf[i] - 'A' + 10) * std::pow(16, buf.length() - i - 1);
        }
        else
            res += (buf[i] - 'a' + 10) * std::pow(16, buf.length() - i - 1);
    }
    return szl::Grammar::toBin(res);
}

std::string szl::Grammar::fromBin(const std::string &in)
{
    std::string buf = in.substr(2, in.length() - 2);
    long res = 0;
    for (std::size_t i = 0; i < buf.length(); i++)
    {
        res += (buf[i] - '0') * std::pow(2, buf.length() - i - 1);
    }
    return szl::Grammar::toBin(res);
}

std::string szl::Grammar::fromDec(const std::string &in)
{
    std::string buf = in;
    long res = 0;
    for (std::size_t i = 0; i < buf.length(); i++)
    {
        res += (buf[i] - '0') * std::pow(10, buf.length() - i - 1);
    }
    return szl::Grammar::toBin(res);
}

std::string szl::Grammar::fromChar(const std::string &in)
{
    std::string buf = in.substr(1, in.length() - 2);
    long res = 0;
    if (buf.length() == 1)
        res = buf[0];
    else if (!buf.length())
        throw szl::SZLException("Attempted to assign empty character");
    else
    {
        if (buf == "\\n")
            res = '\n';
        else if (buf == "\\t")
            res = '\t';
        else if (buf == "\\r")
            res = '\r';
        else if (buf == "\\v")
            res = '\v';
        else if (buf == "\\f")
            res = '\f';
        else
            throw szl::SZLException("Attempted to assign character with unknown value");
    }
    return szl::Grammar::toBin(res);
}

std::string szl::Grammar::fromDecFloat(const std::string &in)
{
    return szl::Grammar::toBinFloat(std::atof(in.c_str()));
}

bool szl::Grammar::isValidNumber(const std::string &in)
{
    if (in == "0")
        return true;
    auto sub = in.substr(0, 2);
    std::string buf = in;
    bool hex = false, oct = false, bin = false;
    if (sub == "0x")
    {
        buf = in.substr(2, in.length() - 2);
        hex = true;
    }
    else if (sub == "0b")
    {
        buf = in.substr(2, in.length() - 2);
        hex = true;
    }
    else if (sub[0] == '0')
    {
        oct = true;
    }
    for (const auto &ch : in)
    {
        if (hex)
        {
            if ((ch < '0' || ch > '9') && (ch < 'a' || ch > 'z') && (ch < 'A' || ch > 'Z'))
                return false;
        }
        else if (oct)
        {
            if (ch < '0' || ch > '7')
                return false;
        }
        else if (bin)
        {
            if (ch != 0 && ch != 1)
                return false;
        }
        else if (ch < '0' || ch > '9')
            return false;
    }
    return true;
}

bool szl::Grammar::isValidFloatNumber(const std::string &in)
{
    if (in == "0")
        return true;
    auto sub = in.substr(0, 2);
    std::string buf = in;
    if (sub == "0x" || sub == "0b")
        return false;
    bool dot = false;
    for (const auto &ch : in)
    {
        if (ch == '.')
        {
            if (dot)
                return false;
            dot = true;
            continue;
        }
        if (ch < '0' || ch > '9')
            return false;
    }
    return true;
}

std::string szl::Grammar::executeSubRules(std::vector<szl::Token> &program, std::size_t &position, szl::Scope &scope, std::vector<std::string> &internalState) const
{
    std::string res;
    for (std::size_t i = 0; i < subRules.size(); i++)
    {
        res = subRules[i]->execute(program, position, scope, internalState);
        if (res.length() > 0)
            return res;
    }
    return "";
}

std::string szl::Grammar::binaryAdd(std::string in1, std::string in2)
{
    std::string res;
    bool carry = false;
    while (in1.length() > in2.length())
    {
        in2 = "0" + in2;
    }
    while (in1.length() < in2.length())
    {
        in1 = "0" + in1;
    }
    for (std::size_t i = in1.length(); i > 0; i--)
    {
        char partial = (in1[i - 1] - '0') + (in2[i - 1] - '0') + (carry ? 1 : 0);
        carry = false;
        if (partial > 1)
            carry = true;
        res = (partial % 2 ? "1" : "0") + res;
    }
    if (carry)
        res = "1" + res;
    return res;
}

std::string szl::Grammar::execute(std::vector<szl::Token> &program, std::size_t &position, szl::Scope &scope, std::vector<std::string> &internalState) const
{
    return executeSubRules(program, position, scope, internalState);
}

void szl::Grammar::addSubRule(Grammar *gRule)
{
    subRules.push_back(gRule);
}

szl::Grammar::Grammar(bool addSubRules)
{
    if (!addSubRules)
        return;
    addSubRule(new szl::GrammarBrackets(true));
    addSubRule(new szl::GrammarSingleVariableDeclaration(true));
    addSubRule(new szl::GrammarSemicolon);
    // TODO
}

szl::Grammar::~Grammar()
{
    for (std::size_t i = 0; i < subRules.size(); i++)
    {
        if (subRules[i] != this)
            delete subRules[i];
    }
}

std::string szl::GrammarSingleVariableDeclaration::execute(std::vector<szl::Token> &program, std::size_t &position, szl::Scope &scope, std::vector<std::string> &internalState) const
{
    if (position + 4 >= program.size())
        return "";
    if (program[position].category != szl::TokenCategory::Keyword)
        return "";
    if (program[position + 1].category != szl::TokenCategory::Identifier)
        return "";
    if (program[position + 2].category != szl::TokenCategory::Keyword)
        return "";

    auto type = program[position].content;
    if (type != "int" && type != "uint" && type != "long" && type != "ulong" && type != "bool" && type != "float" && type != "char" && type != "string")
        return "";
    if (program[position + 2].content != "=")
        return "";

    auto newPos = position + 3;
    auto name = program[position + 1].content;
    std::string subRes = executeSubRules(program, newPos, scope, internalState);
    if (subRes.length() == 0)
        return "";
    if (program[newPos].category != szl::TokenCategory::Punctuation && program[position].content != ";")
    {
        return "";
    }
    position = newPos + 1;

    if (scope.exists(name))
        throw szl::SZLException("Redeclaration of variable " + name);

    internalState.push_back(type);

    // INT/UINT/CHAR/BOOL - HL contains all bytes
    if (type == "int" || type == "uint" || type == "char" || type == "bool")
    {
        scope.insertVariable(name, 2, type);
        return subRes + "PUSH HL\n";
    }

    // LONG/ULONG/FLOAT - DE contains lower bytes, HL upper ones
    if (type == "long" || type == "ulong" || type == "float")
    {
        scope.insertVariable(name, 4, type);
        return subRes + "PUSH DE\nPUSH HL\n";
    }

    // STRING - already head of the stack
    if (type == "string")
    {
        scope.renameHead(name);
    }

    return subRes;
}

szl::GrammarSingleVariableDeclaration::GrammarSingleVariableDeclaration(bool addSubRules) : szl::Grammar()
{
    if (!addSubRules)
        return;
    addSubRule(new szl::GrammarBrackets(true));
    addSubRule(new szl::GrammarTwoLiteralsAddition);
    addSubRule(new szl::GrammarAddition(true));
    addSubRule(new szl::GrammarLiteral);
    addSubRule(new szl::GrammarIdentifier);
    // TODO
}

std::string szl::GrammarTwoLiteralsAddition::execute(std::vector<szl::Token> &program, std::size_t &position, szl::Scope &scope, std::vector<std::string> &internalState) const
{
    if (position + 2 >= program.size())
        return "";
    if (program[position].category != szl::TokenCategory::Literal)
        return "";
    if (program[position + 1].category != szl::TokenCategory::Keyword)
        return "";
    if (program[position + 2].category != szl::TokenCategory::Literal)
        return "";
    if (program[position + 1].content != "+")
        return "";
    auto value1 = program[position].content;
    auto value2 = program[position + 2].content;

    std::string res1, res2;
    bool floats = false;
    if (!szl::Grammar::isValidNumber(value1))
    {
        if (value1[0] != '\'' && value1[0] != '"')
        {
            if (!szl::Grammar::isValidFloatNumber(value1))
                throw szl::SZLException("Attempting to add malformed values");
            else
                floats = true;
        }
    }
    if (!szl::Grammar::isValidNumber(value2))
    {
        if (value2[0] != '\'' && value2[0] != '"')
        {
            if (!szl::Grammar::isValidFloatNumber(value2))
                throw szl::SZLException("Attempting to add malformed values");
            else
                floats = true;
        }
    }
    position += 3;
    int size = 0; // For string addition only
    if (value1[0] == '\'')
    {
        res1 = szl::Grammar::fromChar(value1);
    }
    else if (value1[0] == '"')
    {
        if (value2[0] != '"')
            throw szl::SZLException("Can not add numerical values to string");
        if (value1[value1.length() - 1] != '"')
            throw szl::SZLException("Malformed string chain");
        size = (value1.length() - 2) * 2;
        for (std::size_t i = 1; i < value1.size() - 1; i++)
        {
            res1 += "LD HL,%" + szl::Grammar::fromChar("'" + value1.substr(i, 1) + "'") + "\nPUSH HL\n";
        }
    }
    else if (!floats)
    {
        if (value1.length() == 1)
        {
            res1 = szl::Grammar::fromDec(value1);
        }
        else
        {
            if (value1[1] != 'x' && value1[1] != 'b' && value1[0] == '0')
                res1 = szl::Grammar::fromOct(value1);
            else if (value1[1] == 'x')
                res1 = szl::Grammar::fromHex(value1);
            else if (value1[1] == 'b')
                res1 = szl::Grammar::fromBin(value1);
            else
                res1 = szl::Grammar::fromDec(value1);
        }
    }
    else
    {
        res1 = value1;
    }
    if (value2[0] == '\'')
    {
        res2 = szl::Grammar::fromChar(value1);
    }
    else if (value2[0] == '"')
    {
        if (value1[0] != '"')
            throw szl::SZLException("Can not add numerical values to string");
        if (value2[value2.length() - 1] != '"')
            throw szl::SZLException("Malformed string chain");
        size += (value2.length() - 1) * 2;
        int num = 0;
        while (scope.exists("[STRINGSAVE" + std::to_string(num) + "]"))
        {
            num++;
        }
        scope.insertVariable("[STRINGSAVE" + std::to_string(num) + "]", size, "string");
        for (std::size_t i = 1; i < value2.size() - 1; i++)
        {
            res2 += "LD HL,%" + szl::Grammar::fromChar("'" + value2.substr(i, 1) + "'") + "\nPUSH HL\n";
        }
        internalState.push_back("string");
        return res1 + res2 + "LD HL,%0\nPUSH HL\nLD HL,SP\n";
    }
    else if (!floats)
    {
        if (value2.length() == 1)
        {
            res2 = szl::Grammar::fromDec(value2);
        }
        else
        {
            if (value2[1] != 'x' && value2[1] != 'b' && value2[0] == '0')
                res2 = szl::Grammar::fromOct(value2);
            else if (value2[1] == 'x')
                res2 = szl::Grammar::fromHex(value2);
            else if (value2[1] == 'b')
                res2 = szl::Grammar::fromBin(value2);
            else
                res2 = szl::Grammar::fromDec(value2);
        }
    }
    else
    {
        res2 = value2;
        std::string addRes = szl::Grammar::fromDecFloat(std::to_string(std::atof(res1.c_str()) + std::atof(res2.c_str())));
        internalState.push_back("float");
        return "LD HL,%" +
               addRes.substr(0, 16) + "\nLD DE,%" + addRes.substr(16, 16) + "\n";
    }
    std::string partial = szl::Grammar::binaryAdd(res1, res2);
    if (partial.length() > 32)
        throw szl::SZLException("Added values exceed max allowed size");
    if (partial.length() <= 16)
    {
        internalState.push_back("int");
        return "LD HL,%" + partial + "\n";
    }
    while (partial.length() < 32)
    {
        partial = "0" + partial;
    }
    internalState.push_back("long");
    return "LD HL,%" +
           partial.substr(0, 16) + "\nLD DE,%" + partial.substr(16, 16) + "\n";
}

std::string szl::GrammarBrackets::execute(std::vector<szl::Token> &program, std::size_t &position, szl::Scope &scope, std::vector<std::string> &internalState) const
{
    if (program[position].category != szl::TokenCategory::Bracket)
        return "";
    if (program[position].content != "(")
        return "";
    int brackets = 0;
    for (std::size_t i = position + 1; i + 1 < program.size(); i++)
    {
        if (program[i].category == szl::TokenCategory::Bracket)
        {
            if (program[i].content == ")")
            {
                if (!brackets)
                {
                    if (program[i + 1].category != szl::TokenCategory::Bracket && program[i + 1].category != szl::TokenCategory::Keyword && program[i + 1].category != szl::TokenCategory::Punctuation)
                        return "";
                    std::size_t newPos = position + 1;
                    position = i + 1;
                    return executeSubRules(program, newPos, scope, internalState);
                }
                brackets--;
            }
            else if (program[i].content == "(")
                brackets++;
        }
    }
    return "";
}

szl::GrammarBrackets::GrammarBrackets(bool addSubRules)
{
    if (!addSubRules)
        return;
    addSubRule(new szl::GrammarTwoLiteralsAddition);
    addSubRule(new szl::GrammarLiteral);
    addSubRule(new szl::GrammarIdentifier);
    addSubRule(this);
    // TODO
}

std::string szl::GrammarSemicolon::execute(std::vector<szl::Token> &program, std::size_t &position, szl::Scope &scope, std::vector<std::string> &internalState) const
{
    if (program[position].category != szl::TokenCategory::Punctuation)
        return "";
    if (program[position].content != ";")
        return "";
    position++;
    return "\n";
}

std::string szl::GrammarIdentifier::execute(std::vector<szl::Token> &program, std::size_t &position, szl::Scope &scope, std::vector<std::string> &internalState) const
{
    if (program[position].category != szl::TokenCategory::Identifier)
        return "";
    auto name = program[position].content;
    if (!scope.exists(name))
        return "";
    auto size = scope[name].getStackSize();
    position++;
    internalState.push_back(scope[name].getType());
    if (size == 2)
    {
        return "LD HL,(#" + std::to_string(scope[name].getPosition()) + ")\n";
    }
    if (size == 4)
    {
        return "LD HL,(#" + std::to_string(scope[name].getPosition()) + ")\nLD DE,(#" + std::to_string(scope[name].getPosition() + 2) + ")\n";
    }
    return "LD HL,#" + std::to_string(scope[name].getPosition()) + "\n";
}

std::string szl::GrammarLiteral::execute(std::vector<szl::Token> &program, std::size_t &position, szl::Scope &scope, std::vector<std::string> &internalState) const
{
    if (program[position].category != szl::TokenCategory::Literal)
        return "";
    auto value = program[position].content;
    if (szl::Grammar::isValidNumber(value))
    {
        position++;
        if (value == "0")
        {
            return "LD HL,%0\n";
        }
        if (value.length() > 1)
        {
            std::string res;
            if (value[1] != 'x' && value[1] != 'b' && value[0] == '0')
                res = szl::Grammar::fromOct(value);
            else if (value[1] == 'x')
                res = szl::Grammar::fromHex(value);
            else if (value[1] == 'b')
                res = szl::Grammar::fromBin(value);
            else
                res = szl::Grammar::fromDec(value);
            if (res.length() > 32)
            {
                std::string cmd;
                int size = 0;
                for (std::size_t i = 16; i < res.length(); i += 16)
                {
                    size += 2;
                    cmd += "LD HL,%" + res.substr(res.length() - i, 16) + "\nPUSH HL\n";
                }
                int num = 0;
                while (scope.exists("[LITERALSAVE" + std::to_string(num) + "]"))
                {
                    num++;
                }
                throw szl::SZLException("Literal value exceeds maximum limit");
                // scope.insertVariable("[LITERALSAVE" + std::to_string(num) + "]", size);
                // return cmd + "LD HL,#" + std::to_string(scope["[LITERALSAVE" + std::to_string(num) + "]"].getPosition());
            }
            if (res.length() > 16)
            {
                internalState.push_back("long");
                return "LD DE,%" + res.substr(res.length() - 16, 16) + "\nLD HL,%" + res.substr(0, res.length() - 16) + "\n";
            }
            internalState.push_back("int");
            return "LD HL,%" + res + "\n";
        }
        if (value.length() == 1)
        {
            internalState.push_back("int");
            return "LD HL,%" + szl::Grammar::fromDec(value) + "\n";
        }
    }
    if (value[0] == '\'')
    {
        internalState.push_back("char");
        position++;
        return "LD HL,%" + szl::Grammar::fromChar(value) + "\n";
    }
    if (szl::Grammar::isValidFloatNumber(value))
    {
        internalState.push_back("float");
        position++;
        auto res = szl::Grammar::fromDecFloat(value);
        return "LD DE,%" + res.substr(16, 16) + "\nLD HL,%" + res.substr(0, 16) + "\n";
    }
    if (value == "true" || value == "false")
    {
        internalState.push_back("bool");
        position++;
        if (value == "true")
            return "LD HL,%1\n";
        return "LD HL,%0\n";
    }
    if (value[0] == '"' && value[value.length() - 1] == '"')
    {
        internalState.push_back("string");
        int size = (value.length() - 1) * 2;
        int num = 0;
        while (scope.exists("[STRINGSAVE" + std::to_string(num) + "]"))
        {
            num++;
        }
        scope.insertVariable("[STRINGSAVE" + std::to_string(num) + "]", size, "string");
        position++;
        std::string res;
        for (std::size_t i = 1; i < value.size() - 1; i++)
        {
            res += "LD HL,%" + szl::Grammar::fromChar("'" + value.substr(i, 1) + "'") + "\nPUSH HL\n";
        }
        return res + "LD HL,%0\nPUSH HL\nLD HL,SP\n";
    }
    throw szl::SZLException("Can not load given value");
}

std::string szl::GrammarAddition::execute(std::vector<szl::Token> &program, std::size_t &position, szl::Scope &scope, std::vector<std::string> &internalState) const
{
    std::size_t newPos = position;
    std::string resL = executeSubRules(program, newPos, scope, internalState);
    if (!resL.length())
        return "";
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return "";
    if (program[newPos].content != "+")
        return "";
    int num = 0;
    std::string res;
    szl::Scope *subScope;
    if (internalState.back() == "long" || internalState.back() == "ulong" || internalState.back() == "float")
    {
        while (scope.exists("[ADDSAVE" + std::to_string(num) + "]"))
        {
            num++;
        }
        scope.insertVariable("[ADDSAVE" + std::to_string(num) + "]", 4, internalState.back());
        res += "PUSH DE\nPUSH HL\n";
        subScope = new szl::Scope(4, &res, &scope);
    }
    else if (internalState.back() != "string")
    {
        subScope = new szl::Scope(2, &res, &scope);
    }
    else
    {
        subScope = new szl::Scope(-1, &res, &scope);
    }
    newPos++;
    res += executeSubRules(program, newPos, *subScope, internalState);
    if (!res.length())
        throw szl::SZLException("Addition syntax error");
    position = newPos;
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting addition of differing types");
    internalState.pop_back();
    delete subScope;

    // STRING
    if (internalState.back() == "string")
    {
        throw szl::SZLException("Strings can not be added");
    }

    // INT/UINT/CHAR
    if (internalState.back() == "int" || internalState.back() == "uint" || internalState.back() == "char")
    {
        return resL + "EX DE,HL\n" + res + "ADD HL,DE\n";
    }

    // TODO: TEST
    //  LONG/ULONG
    if (internalState.back() == "long" || internalState.back() == "ulong")
    {
        auto pos = scope["[ADDSAVE" + std::to_string(num) + "]"].getPosition();
        scope.popHead();
        return resL + res + "LD B,H\nLD C,L\nLD HL,(#" + std::to_string(pos + 2) + ")\nADD HL,DE\nEX DE,HL\nLD H,B\nLD L,C\nLD B,D\nLD C,E\nLD DE,(#" + std::to_string(pos) + ")\nADC HL,DE\nEX DE,HL\nLD HL,#4\nADD HL,SP\nLD SP,HL\nEX DE,HL\nLD D,B\nLD E,C\n";
    }

    // FLOAT
    if (internalState.back() == "float")
    {
        auto pos = scope["[ADDSAVE" + std::to_string(num) + "]"].getPosition();
        scope.popHead();
        return resL + res + "LD BC,(#" + std::to_string(pos) + ")\nLD IX,(#" + std::to_string(pos + 2) + ")\nCALL @stdszllib_add_floats\nLD B,H\nLD C,L\nLD HL,#4\nADD HL,SP\nLD SP,HL\nLD H,B\nLD L,C\n";
    }

    // BOOL
    if (internalState.back() == "bool")
    {
        return resL + "LD A,L\n" + res + "OR L\nLD L,A\n";
    }

    throw szl::SZLException("Addition type unrecognized");
}

szl::GrammarAddition::GrammarAddition(bool addSubRules)
{
    if (!addSubRules)
        return;
    addSubRule(new szl::GrammarBrackets);
    addSubRule(new szl::GrammarLiteral);
    addSubRule(new szl::GrammarIdentifier);
    addSubRule(this);
}
