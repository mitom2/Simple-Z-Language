#include "Grammar.hpp"

std::string szl::GrammarSingleVariableDeclarationLiteral::execute(std::vector<szl::Token> &program, std::size_t &position, szl::Scope &scope) const
{
    if (position + 4 >= program.size())
        return "";
    if (program[position].category != szl::TokenCategory::Keyword)
        return "";
    if (program[position + 1].category != szl::TokenCategory::Identifier)
        return "";
    if (program[position + 2].category != szl::TokenCategory::Keyword)
        return "";
    if (program[position + 3].category != szl::TokenCategory::Literal)
        return "";
    if (program[position + 4].category != szl::TokenCategory::Punctuation)
        return "";
    auto type = program[position].content;
    if (type != "int" && type != "uint" && type != "long" && type != "ulong" && type != "bool" && type != "float" && type != "char" && type != "string")
        return "";
    if (program[position + 2].content != "=")
        return "";
    if (program[position + 4].content != ";")
        return "";
    auto value = program[position + 3].content;
    auto name = program[position + 1].content;
    if (scope.exists(name))
        throw szl::SZLException("Redeclaration of variable " + name);

    // INT/UINT/CHAR
    if (type == "int" || type == "uint" || type == "char")
    {
        if (!szl::Grammar::isValidNumber(value))
        {
            if (value[0] != '\'' || value[value.length() - 1] != '\'')
                throw szl::SZLException("Assigned value does not match declared type");
        }
        scope.insertVariable(name, 2);
        position += 5;
        if (value == "0")
        {
            return "XOR H\nXOR L\nPUSH HL\n";
        }
        if (value[0] == '\'')
        {
            return "LD HL,%" + szl::Grammar::fromChar(value) + "\nPUSH HL\n";
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
            if (res.length() > 16)
                throw szl::SZLException("Assigned value exceeds max allowed value for the type");
            return "LD HL,%" + res + "\nPUSH HL\n";
        }
        if (value.length() == 1)
        {
            std::string res = szl::Grammar::fromDec(value);
            if (res.length() > 16)
                throw szl::SZLException("Assigned value exceeds max allowed value for the type");
            return "LD HL,%" + res + "\nPUSH HL\n";
        }
    }

    // LONG/ULONG
    if (type == "long" || type == "ulong")
    {
        if (!szl::Grammar::isValidNumber(value))
            throw szl::SZLException("Assigned value does not match declared type");
        scope.insertVariable(name, 4);
        position += 5;
        if (value == "0")
        {
            return "LD HL,%0\nPUSH HL\n";
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
                throw szl::SZLException("Assigned value exceeds max allowed value for the type");
            std::string upper, lower;
            if (res.length() < 16)
            {
                upper = "0";
                lower = res;
            }
            else
            {
                upper = res.substr(0, res.length() - 16);
                lower = res.substr(res.length() - 16, 16);
            }
            return "LD HL,%" + lower + "\nPUSH HL\nLD HL,%" + upper + "\nPUSH HL\n";
        }
        if (value.length() == 1)
        {
            std::string res = szl::Grammar::fromDec(value);
            std::string upper, lower;
            if (res.length() < 16)
            {
                upper = "0";
                lower = res;
            }
            else
            {
                upper = res.substr(0, res.length() - 16);
                lower = res.substr(res.length() - 16, 16);
            }
            return "LD HL,%" + lower + "\nPUSH HL\nLD HL,%" + upper + "\nPUSH HL\n";
        }
    }

    // FLOAT
    if (type == "float")
    {
        if (!szl::Grammar::isValidNumber(value))
            throw szl::SZLException("Assigned value does not match declared type");
        scope.insertVariable(name, 4);
        position += 5;
        auto res = szl::Grammar::fromDecFloat(value);
        return "LD HL,%" + res.substr(16, 16) + "\nPUSH HL\nLD HL,%" + res.substr(0, 16) + "\nPUSH HL\n";
    }

    // BOOL
    if (type == "bool")
    {
        if (value != "true" && value != "false")
            throw szl::SZLException("Assigned value does not match declared type");
        scope.insertVariable(name, 2);
        position += 5;
        if (value == "true")
            return "LD HL,%1\nPUSH HL\n";
        return "LD HL,%0\nPUSH HL\n";
    }

    // STRING
    if (type == "string")
    {
        if (value[0] != '"' || value[value.length() - 1] != '"')
            throw szl::SZLException("Malformed string declaration");
        int size = (value.length() - 1) * 2;
        scope.insertVariable(name, size);
        position += 5;
        std::string res;
        for (std::size_t i = 1; i < value.size() - 1; i++)
        {
            res += "LD HL,%" + szl::Grammar::fromChar("'" + value.substr(i, 1) + "'") + "\nPUSH HL\n";
        }
        return res + "LD HL,%0\nPUSH HL\n";
    }

    return "";
}

std::string szl::Grammar::toBin(long in)
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
    long res = 0;
    for (std::size_t i = 0; i < buf.length(); i++)
    {
        if (buf[i] <= '9')
            res += (buf[i] - '0') * std::pow(16, buf.length() - i - 1);
        else if (buf[i] <= 'Z')
            res += (buf[i] - 'A' + 10) * std::pow(16, buf.length() - i - 1);
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
    /*std::string buf = in;
    double res = 0;
    std::size_t point = 0;
    bool found = false;
    for (const auto &it : buf)
    {
        if (it == '.')
        {
            found = true;
            break;
        }
        point++;
    }
    if (!found)
        point = buf.length();
    for (std::size_t i = 0; i < point; i++)
    {
        res += (buf[i] - '0') * std::pow(10, point - i - 1);
    }
    if (found)
    {
        for (std::size_t i = point + 1; i < buf.length(); i++)
        {
            res += (buf[i] - '0') * std::pow(10, point - i);
        }
    }
    return szl::Grammar::toBinFloat(res);*/
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

std::string szl::Grammar::executeSubRules(std::vector<szl::Token> &program, std::size_t &position, szl::Scope &scope) const
{
    std::string res;
    for (const auto &it : subRules)
    {
        res = it->execute(program, position, scope);
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
        char partial = in1[i - 1] - '0' + in2[i - 1] - '0' + carry ? 1 : 0;
        carry = false;
        if (partial > 1)
            carry = true;
        res = (partial % 2 ? "1" : "0") + res;
    }
    return res;
}

std::string szl::Grammar::execute(std::vector<szl::Token> &program, std::size_t &position, szl::Scope &scope) const
{
    return executeSubRules(program, position, scope);
}

void szl::Grammar::addSubRule(Grammar *gRule)
{
    subRules.push_back(gRule);
}

szl::Grammar::Grammar(bool addSubRules)
{
    if (!addSubRules)
        return;
    addSubRule(new szl::GrammarBracketsRecursive(true));
    addSubRule(new szl::GrammarSingleVariableDeclarationSubRule(true));
    addSubRule(new szl::GrammarSingleVariableDeclarationIdentifier);
    addSubRule(new szl::GrammarSingleVariableDeclarationLiteral);
    addSubRule(new szl::GrammarSemicolon);
    // TODO
}

szl::Grammar::~Grammar()
{
    for (std::size_t i = 0; i < subRules.size(); i++)
    {
        delete subRules[i];
    }
}

std::string szl::GrammarSingleVariableDeclarationIdentifier::execute(std::vector<szl::Token> &program, std::size_t &position, szl::Scope &scope) const
{
    if (position + 4 >= program.size())
        return "";
    if (program[position].category != szl::TokenCategory::Keyword)
        return "";
    if (program[position + 1].category != szl::TokenCategory::Identifier)
        return "";
    if (program[position + 2].category != szl::TokenCategory::Keyword)
        return "";
    if (program[position + 3].category != szl::TokenCategory::Identifier)
        return "";
    if (program[position + 4].category != szl::TokenCategory::Punctuation)
        return "";
    auto type = program[position].content;
    if (type != "int" && type != "uint" && type != "long" && type != "ulong" && type != "bool" && type != "float" && type != "char" && type != "string")
        return "";
    if (program[position + 2].content != "=")
        return "";
    if (program[position + 4].content != ";")
        return "";
    auto value = program[position + 3].content;
    auto name = program[position + 1].content;
    if (scope.exists(name))
        throw szl::SZLException("Redeclaration of variable " + name);

    // INT/UINT/CHAR/BOOL
    if (type == "int" || type == "uint" || type == "char" || type == "bool")
    {
        if (!scope.exists(value))
            throw szl::SZLException("Assigned variable does not exist");
        if (scope[value].getStackSize() != 2)
            throw szl::SZLException("Size of assigned variable does not match declared type");
        auto pos = scope[value].getPosition();
        scope.insertVariable(name, 2);
        position += 5;
        return "LD HL,(#" + std::to_string(pos) + ")\nPUSH HL\n";
    }

    // LONG/ULONG/FLOAT
    if (type == "long" || type == "ulong" || type == "float")
    {
        if (!scope.exists(value))
            throw szl::SZLException("Assigned variable does not exist");
        if (scope[value].getStackSize() != 4)
            throw szl::SZLException("Size of assigned variable does not match declared type");
        auto pos = scope[value].getPosition();
        scope.insertVariable(name, 4);
        position += 5;
        return "LD HL,(#" + std::to_string(pos + 2) + ")\nPUSH HL\nLD HL,(#" + std::to_string(pos) + ")\nPUSH HL\n";
    }

    // STRING
    if (type == "string")
    {
        if (!scope.exists(value))
            throw szl::SZLException("Assigned variable does not exist");
        auto pos = scope[value].getPosition() + scope[value].getStackSize() - 2;
        position += 5;
        std::string res;
        for (std::size_t i = 0; i < scope[value].getStackSize(); i += 2)
        {
            res += "LD HL,(#" + std::to_string(pos) + ")\nPUSH HL\n";
            pos -= 2;
        }
        scope.insertVariable(name, scope[value].getStackSize());
        return res;
    }

    return "";
}

std::string szl::GrammarSingleVariableDeclarationSubRule::execute(std::vector<szl::Token> &program, std::size_t &position, szl::Scope &scope) const
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
    std::string subRes = executeSubRules(program, newPos, scope);
    if (subRes.length() == 0)
        return "";
    position = newPos;

    if (scope.exists(name))
        throw szl::SZLException("Redeclaration of variable " + name);

    // INT/UINT/CHAR/BOOL
    if (type == "int" || type == "uint" || type == "char" || type == "bool")
    {
        scope.insertVariable(name, 2);
        return subRes + "PUSH HL\n";
    }

    // LONG/ULONG/FLOAT - DE contain lower bytes, HL upper ones
    if (type == "long" || type == "ulong" || type == "float")
    {
        scope.insertVariable(name, 4);
        return subRes + "PUSH DE\nPUSH HL\n";
    }

    // STRING - scope's stack head is string limiting NULL and stack head size is string size
    if (type == "string")
    {
        auto pos = scope.getStackHead()->getPosition() + scope.getStackHead()->getStackSize() - 2;
        std::string res;
        for (std::size_t i = 0; i < scope.getStackHead()->getStackSize(); i += 2)
        {
            res += "LD HL,(#" + std::to_string(pos) + ")\nPUSH HL\n";
            pos -= 2;
        }
        scope.insertVariable(name, scope.getStackHead()->getStackSize());
        return subRes + res;
    }

    return subRes;
}

szl::GrammarSingleVariableDeclarationSubRule::GrammarSingleVariableDeclarationSubRule(bool addSubRules) : szl::Grammar()
{
    if (!addSubRules)
        return;
    addSubRule(new szl::GrammarBracketsRecursive(true));
    addSubRule(new szl::GrammarTwoLiteralsAddition);
    // TODO
}

std::string szl::GrammarTwoLiteralsAddition::execute(std::vector<szl::Token> &program, std::size_t &position, szl::Scope &scope) const
{
    if (position + 3 >= program.size())
        return "";
    if (program[position].category != szl::TokenCategory::Literal)
        return "";
    if (program[position + 1].category != szl::TokenCategory::Keyword)
        return "";
    if (program[position + 2].category != szl::TokenCategory::Literal)
        return "";
    if (program[position + 3].category != szl::TokenCategory::Punctuation && program[position + 3].category != szl::TokenCategory::Bracket)
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
    position += 4;
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
        scope.insertVariable("[STRINGSAVE" + std::to_string(num) + "]", size);
        for (std::size_t i = 1; i < value2.size() - 1; i++)
        {
            res2 += "LD HL,%" + szl::Grammar::fromChar("'" + value2.substr(i, 1) + "'") + "\nPUSH HL\n";
        }
        return res1 + res2 + "LD HL,%0\nPUSH HL\n";
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
        return "LD HL,%" +
               addRes.substr(0, 16) + "\nLD DE,%" + addRes.substr(16, 16) + "\n";
    }
    std::string partial = szl::Grammar::binaryAdd(res1, res2);
    if (partial.length() > 32)
        throw szl::SZLException("Added values exceed max allowed size");
    if (partial.length() <= 16)
        return "LD HL,%" + partial + "\nLD DE,%0\n";
    while (partial.length() < 32)
    {
        partial = "0" + partial;
    }
    return "LD HL,%" +
           partial.substr(0, 16) + "\nLD DE,%" + partial.substr(16, 16) + "\n";
}

std::string szl::GrammarBracketsRecursive::execute(std::vector<szl::Token> &program, std::size_t &position, szl::Scope &scope) const
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
                    return executeSubRules(program, newPos, scope);
                }
                brackets--;
            }
            else if (program[i].content == "(")
                brackets++;
        }
    }
    return "";
}

szl::GrammarBracketsRecursive::GrammarBracketsRecursive(bool addSubRules)
{
    if (!addSubRules)
        return;
    addSubRule(new szl::GrammarTwoLiteralsAddition);
    // TODO
}

std::string szl::GrammarSemicolon::execute(std::vector<szl::Token> &program, std::size_t &position, szl::Scope &scope) const
{
    if (program[position].category != szl::TokenCategory::Punctuation)
        return "";
    if (program[position].content != ";")
        return "";
    position++;
    return "\n";
}
