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

std::string szl::Grammar::executeSubRules(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
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

szl::Grammar *szl::Grammar::getGrammar(const std::string &name)
{
    if (root != nullptr)
    {
        return root->getGrammar(name);
    }
    if (grammars.count(name) > 0)
        return grammars.at(name);
    if (name == "all")
        return this;
    throw szl::SZLException("Grammar rule '" + name + "' not found");
}

std::string szl::Grammar::compileScope(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::string res;
    scope.back().changeCode(&res);
    while (position < program.size())
    {
        std::vector<std::string> internal;
        auto code = root->execute(program, position, scope, internal);
        if (code != "")
        {
            if (code == ";Close scope\n")
                return res;
            res += code;
            continue;
        }
        throw szl::SZLException("Syntax error");
    }
    throw szl::SZLException("Scope not concluded at the end of the program");
}

std::string szl::Grammar::returnWithClear(int amount, std::vector<std::string> &internalState) const
{
    internalState.resize(internalState.size() - amount);
    return "";
}

void szl::Grammar::initialize()
{
    if (root != nullptr)
        return;
    addSubRule("semicolon");
    addSubRule("single variable declaration");
    addSubRule("if");
    addSubRule("while");
    addSubRule("for");
    addSubRule("scope closed");
    addSubRule("assignment");
    addSubRule("function declaration");
    addSubRule("return");
    addSubRule("function call");
    addSubRule("lock");
    addSubRule("unlock");
    addSubRule("out");
    addSubRule("arrow");
    addSubRule("free");
    for (auto &it : grammars)
    {
        it.second->initialize();
    }
}

std::string szl::Grammar::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    return executeSubRules(program, position, scope, internalState);
}

void szl::Grammar::addSubRule(const std::string id)
{
    if (root != nullptr)
        subRules.push_back(root->getGrammar(id));
    else
        subRules.push_back(getGrammar(id));
}

szl::Grammar::Grammar(Grammar *root) : root(root)
{
    if (root != nullptr)
        return;
    grammars.emplace("semicolon", new szl::GrammarSemicolon(this));
    grammars.emplace("single variable declaration", new szl::GrammarSingleVariableDeclaration(this));
    grammars.emplace("two literals addition", new szl::GrammarTwoLiteralsAddition(this));
    grammars.emplace("brackets", new szl::GrammarBrackets(this));
    grammars.emplace("identifier", new szl::GrammarIdentifier(this));
    grammars.emplace("literal", new szl::GrammarLiteral(this));
    grammars.emplace("addition", new szl::GrammarAddition(this));
    grammars.emplace("subtraction", new szl::GrammarSubtraction(this));
    grammars.emplace("multiplication", new szl::GrammarMultiplication(this));
    grammars.emplace("division", new szl::GrammarDivision(this));
    grammars.emplace("and", new szl::GrammarAnd(this));
    grammars.emplace("or", new szl::GrammarOr(this));
    grammars.emplace("xor", new szl::GrammarXor(this));
    grammars.emplace("modulo", new szl::GrammarModulo(this));
    grammars.emplace("not", new szl::GrammarNot(this));
    grammars.emplace("negation", new szl::GrammarNegation(this));
    grammars.emplace("shift left", new szl::GrammarShiftLeft(this));
    grammars.emplace("shift right", new szl::GrammarShiftRight(this));
    grammars.emplace("not equal", new szl::GrammarNotEqual(this));
    grammars.emplace("equal", new szl::GrammarEqual(this));
    grammars.emplace("greater", new szl::GrammarGreater(this));
    grammars.emplace("greater or equal", new szl::GrammarGreaterOrEqual(this));
    grammars.emplace("less", new szl::GrammarLess(this));
    grammars.emplace("less or equal", new szl::GrammarLessOrEqual(this));
    grammars.emplace("if", new szl::GrammarIf(this));
    grammars.emplace("scope closed", new szl::GrammarScopeClosed(this));
    grammars.emplace("while", new szl::GrammarWhile(this));
    grammars.emplace("for", new szl::GrammarFor(this));
    grammars.emplace("assignment", new szl::GrammarAssignment(this));
    grammars.emplace("function declaration", new szl::GrammarFunctionDeclaration(this));
    grammars.emplace("return", new szl::GrammarReturn(this));
    grammars.emplace("function call", new szl::GrammarFunctionCall(this));
    grammars.emplace("lock", new szl::GrammarLock(this));
    grammars.emplace("unlock", new szl::GrammarUnlock(this));
    grammars.emplace("in", new szl::GrammarIn(this));
    grammars.emplace("out", new szl::GrammarOut(this));
    grammars.emplace("at", new szl::GrammarAt(this));
    grammars.emplace("arrow", new szl::GrammarArrow(this));
    grammars.emplace("questioned at", new szl::GrammarQuestionedAt(this));
    grammars.emplace("alloc", new szl::GrammarAlloc(this));
    grammars.emplace("free", new szl::GrammarFree(this));
}

szl::Grammar::~Grammar()
{
    for (auto &[key, value] : grammars)
    {
        delete value;
    }
}

std::string szl::GrammarSingleVariableDeclaration::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
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
    if (program[position + 2].content != "=")
        return "";
    if (type != "int" && type != "uint" && type != "long" && type != "ulong" && type != "bool" && type != "float" && type != "char" && type != "string" && szl::objectTypes.count(type) == 0)
        return "";

    auto newPos = position + 3;
    auto name = program[position + 1].content;
    std::string subRes = executeSubRules(program, newPos, scope, internalState);
    if (subRes.length() == 0)
        return "";
    // if (program[newPos].category != szl::TokenCategory::Punctuation && program[position].content != ";")
    // {
    //     return "";
    // }
    // position = newPos + 1;
    position = newPos;

    if (scope.back().exists(name))
        throw szl::SZLException("Redeclaration of variable " + name);
    if (szl::functions.count(name) > 0)
        throw szl::SZLException("Redeclaration of function " + name);

    if (internalState.back() != type)
        throw szl::SZLException("Declaring variable of type '" + type + "', but passing value of type '" + internalState.back() + "'");

    // INT/UINT/CHAR/BOOL - HL contains all bytes
    if (type == "int" || type == "uint" || type == "char" || type == "bool")
    {
        scope.back().insertVariable(name, 2, type);
        return subRes + "PUSH HL\n";
    }

    // LONG/ULONG/FLOAT - DE contains lower bytes, HL upper ones
    if (type == "long" || type == "ulong" || type == "float")
    {
        scope.back().insertVariable(name, 4, type);
        return subRes + "PUSH DE\nPUSH HL\n";
    }

    // STRING - already head of the stack
    if (type == "string")
    {
        scope.back().renameHead(name);
        return;
    }

    // OBJECT - HL contains address of the object
    if (szl::objectTypes.count(type) > 0)
    {
        auto object = szl::objectTypes[type];
        if (object.getContents().count("operator=") > 0)
            return subRes + "CALL " + object.getContents()["operator="];
        throw szl::SZLException("Object of type '" + type + "' does not have operator= defined");
    }
    else
    {
        throw szl::SZLException("Unknown type '" + type + "' in variable declaration");
    }

    return subRes;
}

szl::GrammarSingleVariableDeclaration::GrammarSingleVariableDeclaration(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarSingleVariableDeclaration::initialize()
{
    addSubRule("questioned at");
    addSubRule("two literals addition");
    addSubRule("brackets");
    addSubRule("addition");
    addSubRule("subtraction");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("and");
    addSubRule("or");
    addSubRule("xor");
    addSubRule("modulo");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("shift left");
    addSubRule("shift right");
    addSubRule("not equal");
    addSubRule("equal");
    addSubRule("greater");
    addSubRule("greater or equal");
    addSubRule("less");
    addSubRule("less or equal");
    addSubRule("identifier");
    addSubRule("literal");
    addSubRule("in");
    addSubRule("alloc");
    addSubRule("function call");
}

std::string szl::GrammarAssignment::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    if (position + 3 >= program.size())
        return "";
    if (program[position].category != szl::TokenCategory::Identifier)
        return "";
    if (program[position + 1].category != szl::TokenCategory::Keyword)
        return "";

    auto name = program[position].content;
    if (program[position + 1].content != "=")
        return "";

    auto newPos = position + 2;
    std::string subRes = executeSubRules(program, newPos, scope, internalState);
    if (subRes.length() == 0)
        return "";
    if (program[newPos].category != szl::TokenCategory::Punctuation && program[position].content != ";")
    {
        return returnWithClear(1, internalState);
    }
    position = newPos + 1;

    if (!scope.back().exists(name))
        throw szl::SZLException("Assigning to variable " + name + ", which does not exist in this scope");

    internalState.push_back(scope.back()[name].getType());
    auto size = scope.back()[name].getStackSize();
    if (size == 2)
        return subRes + "LD (#" + std::to_string(scope.back()[name].getPosition()) + "),HL\n";
    if (size == 4)
        return subRes + "LD (#" + std::to_string(scope.back()[name].getPosition()) + "),HL\n" + "LD (#" + std::to_string(scope.back()[name].getPosition() + 2) + "),DE\n";
    else
    {
        // TODO
    }
}

szl::GrammarAssignment::GrammarAssignment(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarAssignment::initialize()
{
    addSubRule("two literals addition");
    addSubRule("brackets");
    addSubRule("addition");
    addSubRule("subtraction");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("and");
    addSubRule("or");
    addSubRule("xor");
    addSubRule("modulo");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("shift left");
    addSubRule("shift right");
    addSubRule("not equal");
    addSubRule("equal");
    addSubRule("greater");
    addSubRule("greater or equal");
    addSubRule("less");
    addSubRule("less or equal");
    addSubRule("identifier");
    addSubRule("literal");
    addSubRule("alloc");
    addSubRule("function call");
}

std::string szl::GrammarTwoLiteralsAddition::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
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
        while (scope.back().exists("[STRINGSAVE" + std::to_string(num) + "]"))
        {
            num++;
        }
        scope.back().insertVariable("[STRINGSAVE" + std::to_string(num) + "]", size, "string");
        for (std::size_t i = 1; i < value2.size() - 1; i++)
        {
            res2 += "LD HL,%" + szl::Grammar::fromChar("'" + value2.substr(i, 1) + "'") + "\nPUSH HL\n";
        }
        internalState.push_back("string");
        return "LD HL,%0\nPUSH HL\n" + res1 + res2 + "LD HL,%0\nPUSH HL\nLD HL,SP\n";
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

szl::GrammarTwoLiteralsAddition::GrammarTwoLiteralsAddition(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarTwoLiteralsAddition::initialize()
{
}

std::string szl::GrammarBrackets::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
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

szl::GrammarBrackets::GrammarBrackets(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarBrackets::initialize()
{
}

std::string szl::GrammarSemicolon::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    if (program[position].category != szl::TokenCategory::Punctuation)
        return "";
    if (program[position].content != ";")
        return "";
    position++;
    return "\n";
}

szl::GrammarSemicolon::GrammarSemicolon(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarSemicolon::initialize()
{
}

std::string szl::GrammarIdentifier::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    if (program[position].category != szl::TokenCategory::Identifier)
        return "";
    auto name = program[position].content;
    if (!scope.back().exists(name))
        return "";
    auto size = scope.back()[name].getStackSize();
    position++;
    internalState.push_back(scope.back()[name].getType());
    if (size == 2)
    {
        return "LD HL,(#" + std::to_string(scope.back()[name].getPosition()) + ")\n";
    }
    if (size == 4)
    {
        return "LD HL,(#" + std::to_string(scope.back()[name].getPosition()) + ")\nLD DE,(#" + std::to_string(scope.back()[name].getPosition() + 2) + ")\n";
    }
    return "LD HL,#" + std::to_string(scope.back()[name].getPosition()) + "\n";
}

szl::GrammarIdentifier::GrammarIdentifier(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarIdentifier::initialize()
{
}

std::string szl::GrammarLiteral::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    if (program[position].category != szl::TokenCategory::Literal)
        return "";
    auto value = program[position].content;
    if (szl::Grammar::isValidNumber(value))
    {
        position++;
        if (value == "0")
        {
            internalState.push_back("int");
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
                while (scope.back().exists("[LITERALSAVE" + std::to_string(num) + "]"))
                {
                    num++;
                }
                throw szl::SZLException("Literal value exceeds maximum limit");
                // scope.back().insertVariable("[LITERALSAVE" + std::to_string(num) + "]", size);
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
        while (scope.back().exists("[STRINGSAVE" + std::to_string(num) + "]"))
        {
            num++;
        }
        scope.back().insertVariable("[STRINGSAVE" + std::to_string(num) + "]", size, "string");
        position++;
        std::string res = "LD HL,%0\nPUSH HL\n";
        for (std::size_t i = 1; i < value.size() - 1; i++)
        {
            res += "LD HL,%" + szl::Grammar::fromChar("'" + value.substr(i, 1) + "'") + "\nPUSH HL\n";
        }
        return res + "LD HL,%0\nPUSH HL\nLD HL,SP\n";
    }
    throw szl::SZLException("Can not load given value");
}

szl::GrammarLiteral::GrammarLiteral(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarLiteral::initialize()
{
}

std::string szl::GrammarAddition::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::size_t newPos = position;
    std::string resL = executeSubRules(program, newPos, scope, internalState);
    if (!resL.length())
        return "";
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return returnWithClear(1, internalState);
    if (program[newPos].content != "+")
        return returnWithClear(1, internalState);
    int num = 0;
    std::string res;
    newPos++;
    res += executeSubRules(program, newPos, scope, internalState);
    if (!res.length())
        throw szl::SZLException("Addition syntax error");
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("Addition syntax error");
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting addition of differing types");
    internalState.pop_back();

    // STRING
    if (internalState.back() == "string")
    {
        throw szl::SZLException("Strings can not be added");
    }

    // INT/UINT/CHAR
    if (internalState.back() == "int" || internalState.back() == "uint" || internalState.back() == "char")
    {
        return resL + "PUSH HL\n" + res + "POP DE\nADD HL,DE\n";
    }

    //  LONG/ULONG
    if (internalState.back() == "long" || internalState.back() == "ulong")
    {
        return resL + "PUSH HL\nPUSH DE\n" + res + "EXX\nPOP BC\nEXX\nPOP BC\nCALL @stdszllib_add_32bit\n";
    }

    // FLOAT
    if (internalState.back() == "float")
    {
        return resL + "PUSH HL\nPUSH DE\n" + res + "POP IX\nPOP BC\nXOR A\nCALL @stdszllib_add_sub_floats\n";
    }

    // BOOL
    if (internalState.back() == "bool")
    {
        throw szl::SZLException("Bools can not be added");
    }

    throw szl::SZLException("Addition type unrecognized");
}

szl::GrammarAddition::GrammarAddition(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarAddition::initialize()
{
    addSubRule("two literals addition");
    addSubRule("brackets");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("modulo");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("shift left");
    addSubRule("shift right");
    addSubRule("not equal");
    addSubRule("equal");
    addSubRule("greater");
    addSubRule("greater or equal");
    addSubRule("less");
    addSubRule("less or equal");
    addSubRule("identifier");
    addSubRule("literal");
}

std::string szl::GrammarSubtraction::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::size_t newPos = position;
    std::string resL = executeSubRules(program, newPos, scope, internalState);
    if (!resL.length())
        return "";
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return returnWithClear(1, internalState);
    if (program[newPos].content != "-")
        return returnWithClear(1, internalState);
    int num = 0;
    std::string res;
    newPos++;
    res += executeSubRules(program, newPos, scope, internalState);
    if (!res.length())
        throw szl::SZLException("Subtraction syntax error");
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("Subtraction syntax error");
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting subtraction of differing types");
    internalState.pop_back();

    // STRING
    if (internalState.back() == "string")
    {
        throw szl::SZLException("Strings can not be subtracted");
    }

    // INT/UINT/CHAR
    if (internalState.back() == "int" || internalState.back() == "uint" || internalState.back() == "char")
    {
        return resL + "PUSH HL\n" + res + "POP DE\nOR A\nSBC HL,DE\n";
    }

    //  LONG/ULONG
    if (internalState.back() == "long" || internalState.back() == "ulong")
    {
        return resL + "PUSH HL\nPUSH DE\n" + res + "EXX\nPOP BC\nEXX\nPOP BC\nCALL @stdszllib_subtract_32bit\n";
    }

    // FLOAT
    if (internalState.back() == "float")
    {
        return resL + "PUSH HL\nPUSH DE\n" + res + "POP IX\nPOP BC\nLD A,%1\nCALL @stdszllib_add_sub_floats\n";
    }

    // BOOL
    if (internalState.back() == "bool")
    {
        throw szl::SZLException("Bools can not be subtracted");
    }

    throw szl::SZLException("Subtraction type unrecognized");
}

szl::GrammarSubtraction::GrammarSubtraction(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarSubtraction::initialize()
{
}

std::string szl::GrammarMultiplication::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::size_t newPos = position;
    std::string resL = executeSubRules(program, newPos, scope, internalState);
    if (!resL.length())
        return "";
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return returnWithClear(1, internalState);
    if (program[newPos].content != "*")
        return returnWithClear(1, internalState);
    int num = 0;
    std::string res;
    newPos++;
    res += executeSubRules(program, newPos, scope, internalState);
    if (!res.length())
        throw szl::SZLException("Multiplication syntax error");
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("Multiplication syntax error");
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting multiplication of differing types");
    internalState.pop_back();

    // STRING
    if (internalState.back() == "string")
    {
        throw szl::SZLException("Strings can not be multiplied");
    }

    // UINT/CHAR
    if (internalState.back() == "uint" || internalState.back() == "char")
    {
        return resL + "PUSH HL\n" + res + "POP DE\nCALL @stdszllib_multiply_uint\n";
    }

    // INT
    if (internalState.back() == "int")
    {
        return resL + "PUSH HL\n" + res + "POP DE\nCALL @stdszllib_multiply_int\n";
    }

    //  ULONG
    if (internalState.back() == "ulong")
    {
        return resL + "PUSH HL\nPUSH DE\n" + res + "EXX\nPOP BC\nEXX\nPOP BC\nCALL @stdszllib_multiply_ulong\n";
    }

    //  LONG
    if (internalState.back() == "long")
    {
        return resL + "PUSH HL\nPUSH DE\n" + res + "EXX\nPOP BC\nEXX\nPOP BC\nCALL @stdszllib_multiply_long\n";
    }

    // FLOAT
    if (internalState.back() == "float")
    {
        // TODO
    }

    // BOOL
    if (internalState.back() == "bool")
    {
        throw szl::SZLException("Bools can not be multiplied");
    }

    throw szl::SZLException("Multiplication type unrecognized");
}

szl::GrammarMultiplication::GrammarMultiplication(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarMultiplication::initialize()
{
}

std::string szl::GrammarDivision::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::size_t newPos = position;
    std::string resL = executeSubRules(program, newPos, scope, internalState);
    if (!resL.length())
        return "";
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return returnWithClear(1, internalState);
    if (program[newPos].content != "/")
        return returnWithClear(1, internalState);
    int num = 0;
    std::string res;
    newPos++;
    res += executeSubRules(program, newPos, scope, internalState);
    if (!res.length())
        throw szl::SZLException("Division syntax error");
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("Division syntax error");
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting division of differing types");
    internalState.pop_back();

    // STRING
    if (internalState.back() == "string")
    {
        throw szl::SZLException("Strings can not be divided");
    }

    // UINT/CHAR
    if (internalState.back() == "uint" || internalState.back() == "char")
    {
        return resL + "PUSH HL\n" + res + "POP DE\nCALL @stdszllib_divide_uint\n";
    }

    // INT
    if (internalState.back() == "int")
    {
        return resL + "PUSH HL\n" + res + "POP DE\nCALL @stdszllib_divide_int\n";
    }

    //  ULONG
    if (internalState.back() == "ulong")
    {
        return resL + "PUSH HL\nPUSH DE\n" + res + "EXX\nPOP BC\nEXX\nPOP BC\nCALL @stdszllib_divide_ulong\n";
    }

    //  LONG
    if (internalState.back() == "long")
    {
        return resL + "PUSH HL\nPUSH DE\n" + res + "EXX\nPOP BC\nEXX\nPOP BC\nCALL @stdszllib_divide_long\n";
    }

    // FLOAT
    if (internalState.back() == "float")
    {
        // TODO
    }

    // BOOL
    if (internalState.back() == "bool")
    {
        throw szl::SZLException("Bools can not be divided");
    }

    throw szl::SZLException("Division type unrecognized");
}

szl::GrammarDivision::GrammarDivision(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarDivision::initialize()
{
}

std::string szl::GrammarAnd::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::size_t newPos = position;
    std::string resL = executeSubRules(program, newPos, scope, internalState);
    if (!resL.length())
        return "";
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return returnWithClear(1, internalState);
    if (program[newPos].content != "&")
        return returnWithClear(1, internalState);
    int num = 0;
    std::string res;
    newPos++;
    res += executeSubRules(program, newPos, scope, internalState);
    if (!res.length())
        throw szl::SZLException("AND syntax error");
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("AND  syntax error");
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting AND with parameters of differing types");
    internalState.pop_back();

    // STRING
    if (internalState.back() == "string")
    {
        throw szl::SZLException("Strings can not be arguments of AND");
    }

    // INT/UINT/CHAR
    if (internalState.back() == "int" || internalState.back() == "uint" || internalState.back() == "char")
    {
        return resL + "PUSH HL\n" + res + "POP DE\nCALL @stdszllib_and_16bit\n";
    }

    //  LONG/ULONG
    if (internalState.back() == "long" || internalState.back() == "ulong")
    {
        return resL + "PUSH HL\nPUSH DE\n" + res + "EXX\nPOP BC\nEXX\nPOP BC\nCALL @stdszllib_and_32bit\n";
    }

    // FLOAT
    if (internalState.back() == "float")
    {
        throw szl::SZLException("Floats can not be arguments of AND");
    }

    // BOOL
    if (internalState.back() == "bool")
    {
        return resL + "PUSH HL\n" + res + "POP DE\nCALL @stdszllib_and_bool\n";
    }

    throw szl::SZLException("AND operation type unrecognized");
}

szl::GrammarAnd::GrammarAnd(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarAnd::initialize()
{
    addSubRule("two literals addition");
    addSubRule("brackets");
    addSubRule("addition");
    addSubRule("subtraction");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("modulo");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("shift left");
    addSubRule("shift right");
    addSubRule("not equal");
    addSubRule("equal");
    addSubRule("greater");
    addSubRule("greater or equal");
    addSubRule("less");
    addSubRule("less or equal");
    addSubRule("identifier");
    addSubRule("literal");
    addSubRule("in");
    addSubRule("function call");
}

std::string szl::GrammarOr::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::size_t newPos = position;
    std::string resL = executeSubRules(program, newPos, scope, internalState);
    if (!resL.length())
        return "";
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return returnWithClear(1, internalState);
    if (program[newPos].content != "|")
        return returnWithClear(1, internalState);
    int num = 0;
    std::string res;
    newPos++;
    res += executeSubRules(program, newPos, scope, internalState);
    if (!res.length())
        throw szl::SZLException("OR syntax error");
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("OR  syntax error");
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting OR with parameters of differing types");
    internalState.pop_back();

    // STRING
    if (internalState.back() == "string")
    {
        throw szl::SZLException("Strings can not be arguments of OR");
    }

    // INT/UINT/CHAR
    if (internalState.back() == "int" || internalState.back() == "uint" || internalState.back() == "char")
    {
        return resL + "PUSH HL\n" + res + "POP DE\nCALL @stdszllib_or_16bit\n";
    }

    //  LONG/ULONG
    if (internalState.back() == "long" || internalState.back() == "ulong")
    {
        return resL + "PUSH HL\nPUSH DE\n" + res + "EXX\nPOP BC\nEXX\nPOP BC\nCALL @stdszllib_or_32bit\n";
    }

    // FLOAT
    if (internalState.back() == "float")
    {
        throw szl::SZLException("Floats can not be arguments of OR");
    }

    // BOOL
    if (internalState.back() == "bool")
    {
        return resL + "PUSH HL\n" + res + "POP DE\nCALL @stdszllib_or_bool\n";
    }

    throw szl::SZLException("OR operation type unrecognized");
}

szl::GrammarOr::GrammarOr(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarOr::initialize()
{
}

std::string szl::GrammarXor::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::size_t newPos = position;
    std::string resL = executeSubRules(program, newPos, scope, internalState);
    if (!resL.length())
        return "";
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return returnWithClear(1, internalState);
    if (program[newPos].content != "^")
        return returnWithClear(1, internalState);
    int num = 0;
    std::string res;
    newPos++;
    res += executeSubRules(program, newPos, scope, internalState);
    if (!res.length())
        throw szl::SZLException("XOR syntax error");
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("XOR  syntax error");
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting XOR with parameters of differing types");
    internalState.pop_back();

    // STRING
    if (internalState.back() == "string")
    {
        throw szl::SZLException("Strings can not be arguments of XOR");
    }

    // INT/UINT/CHAR
    if (internalState.back() == "int" || internalState.back() == "uint" || internalState.back() == "char")
    {
        return resL + "PUSH HL\n" + res + "POP DE\nCALL @stdszllib_xor_16bit\n";
    }

    //  LONG/ULONG
    if (internalState.back() == "long" || internalState.back() == "ulong")
    {
        return resL + "PUSH HL\nPUSH DE\n" + res + "EXX\nPOP BC\nEXX\nPOP BC\nCALL @stdszllib_xor_32bit\n";
    }

    // FLOAT
    if (internalState.back() == "float")
    {
        throw szl::SZLException("Floats can not be arguments of XOR");
    }

    // BOOL
    if (internalState.back() == "bool")
    {
        return resL + "PUSH HL\n" + res + "POP DE\nCALL @stdszllib_xor_bool\n";
    }

    throw szl::SZLException("XOR operation type unrecognized");
}

szl::GrammarXor::GrammarXor(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarXor::initialize()
{
}

std::string szl::GrammarModulo::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::size_t newPos = position;
    std::string resL = executeSubRules(program, newPos, scope, internalState);
    if (!resL.length())
        return "";
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return returnWithClear(1, internalState);
    if (program[newPos].content != "%")
        return returnWithClear(1, internalState);
    int num = 0;
    std::string res;
    newPos++;
    res += executeSubRules(program, newPos, scope, internalState);
    if (!res.length())
        throw szl::SZLException("Modulo syntax error");
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("Modulo syntax error");
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting to get modulo of differing types");
    internalState.pop_back();

    // STRING
    if (internalState.back() == "string")
    {
        throw szl::SZLException("Strings can not be arguments of modulo");
    }

    // UINT/CHAR
    if (internalState.back() == "uint" || internalState.back() == "char")
    {
        return resL + "PUSH HL\n" + res + "POP DE\nCALL @stdszllib_divide_uint\nEX DE,HL\n";
    }

    // INT
    if (internalState.back() == "int")
    {
        return resL + "PUSH HL\n" + res + "POP DE\nCALL @stdszllib_divide_int\nEX DE,HL\n";
    }

    //  ULONG
    if (internalState.back() == "ulong")
    {
        return resL + "PUSH HL\nPUSH DE\n" + res + "EXX\nPOP BC\nEXX\nPOP BC\nCALL @stdszllib_divide_ulong\nLD H,B\nLD L,C\nEXX\nLD A,B\nEXX\nLD D,A\nEXX\nLD A,C\nEXX\nLD E,A\n";
    }

    //  LONG
    if (internalState.back() == "long")
    {
        return resL + "PUSH HL\nPUSH DE\n" + res + "EXX\nPOP BC\nEXX\nPOP BC\nCALL @stdszllib_divide_long\nLD H,B\nLD L,C\nEXX\nLD A,B\nEXX\nLD D,A\nEXX\nLD A,C\nEXX\nLD E,A\n";
    }

    // FLOAT
    if (internalState.back() == "float")
    {
        throw szl::SZLException("Floats can not be arguments of modulo");
    }

    // BOOL
    if (internalState.back() == "bool")
    {
        throw szl::SZLException("Bools can not be arguments of modulo");
    }

    throw szl::SZLException("Modulo type unrecognized");
}

szl::GrammarModulo::GrammarModulo(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarModulo::initialize()
{
}

std::string szl::GrammarNot::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::size_t newPos = position;
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return "";
    if (program[newPos].content != "~")
        return "";
    std::string res;
    newPos++;
    res += executeSubRules(program, newPos, scope, internalState);
    if (!res.length())
        throw szl::SZLException("NOT syntax error");
    position = newPos;
    if (internalState.size() < 1)
        throw szl::SZLException("NOT syntax error");

    // STRING
    if (internalState.back() == "string")
    {
        throw szl::SZLException("Strings can not be arguments of NOT");
    }

    // INT/UINT/CHAR
    if (internalState.back() == "int" || internalState.back() == "uint" || internalState.back() == "char")
    {
        return res + "CALL @stdszllib_not_16bit\n";
    }

    //  LONG/ULONG
    if (internalState.back() == "long" || internalState.back() == "ulong")
    {
        return res + "CALL @stdszllib_not_32bit\n";
    }

    // FLOAT
    if (internalState.back() == "float")
    {
        throw szl::SZLException("Floats can not be arguments of NOT");
    }

    // BOOL
    if (internalState.back() == "bool")
    {
        return res + "CALL @stdszllib_negation_16bit\n";
    }

    throw szl::SZLException("NOT operation type unrecognized");
}

szl::GrammarNot::GrammarNot(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarNot::initialize()
{
}

std::string szl::GrammarNegation::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::size_t newPos = position;
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return "";
    if (program[newPos].content != "!")
        return "";
    std::string res;
    newPos++;
    res += executeSubRules(program, newPos, scope, internalState);
    if (!res.length())
        throw szl::SZLException("Negation syntax error");
    position = newPos;
    if (internalState.size() < 1)
        throw szl::SZLException("Negation syntax error");

    // STRING
    if (internalState.back() == "string")
    {
        throw szl::SZLException("Strings can not be arguments of negation");
    }

    // INT/UINT/CHAR
    if (internalState.back() == "int" || internalState.back() == "uint" || internalState.back() == "char")
    {
        internalState.back() = "bool";
        return res + "CALL @stdszllib_negation_16bit\n";
    }

    //  LONG/ULONG
    if (internalState.back() == "long" || internalState.back() == "ulong")
    {
        internalState.back() = "bool";
        return res + "CALL @stdszllib_negation_32bit\n";
    }

    // FLOAT
    if (internalState.back() == "float")
    {
        throw szl::SZLException("Floats can not be arguments of negation");
    }

    // BOOL
    if (internalState.back() == "bool")
    {
        return res + "CALL @stdszllib_negation_16bit\n";
    }

    throw szl::SZLException("Negation operation type unrecognized");
}

szl::GrammarNegation::GrammarNegation(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarNegation::initialize()
{
}

std::string szl::GrammarShiftLeft::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::size_t newPos = position;
    std::string resL = executeSubRules(program, newPos, scope, internalState);
    if (!resL.length())
        return "";
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return returnWithClear(1, internalState);
    if (program[newPos].content != "<<")
        return returnWithClear(1, internalState);
    int num = 0;
    std::string res;
    newPos++;
    res += executeSubRules(program, newPos, scope, internalState);
    if (!res.length())
        throw szl::SZLException("Left shift syntax error");
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("Left shift syntax error");
    if (internalState.back() != "uint")
        throw szl::SZLException("Second argument of left shift can only be of type uint");
    internalState.pop_back();

    // STRING
    if (internalState.back() == "string")
    {
        throw szl::SZLException("Strings can not be arguments of left shift");
    }

    // INT/UINT/CHAR
    if (internalState.back() == "int" || internalState.back() == "uint" || internalState.back() == "char")
    {
        return resL + "PUSH HL\n" + res + "POP DE\nEX DE,HL\nCALL @stdszllib_left_shift_16bit\n";
    }

    //  LONG/ULONG
    if (internalState.back() == "long" || internalState.back() == "ulong")
    {
        return resL + "PUSH HL\nPUSH DE\n" + res + "LD B,H\nLD C,L\nPOP DE\nPOP HL\nCALL @stdszllib_left_shift_32bit\n";
    }

    // FLOAT
    if (internalState.back() == "float")
    {
        throw szl::SZLException("Floats can not be arguments of left shift");
    }

    // BOOL
    if (internalState.back() == "bool")
    {
        throw szl::SZLException("Bools can not be arguments of left shift");
    }

    throw szl::SZLException("Left shift type unrecognized");
}

szl::GrammarShiftLeft::GrammarShiftLeft(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarShiftLeft::initialize()
{
}

std::string szl::GrammarShiftRight::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::size_t newPos = position;
    std::string resL = executeSubRules(program, newPos, scope, internalState);
    if (!resL.length())
        return "";
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return returnWithClear(1, internalState);
    if (program[newPos].content != ">>")
        return returnWithClear(1, internalState);
    int num = 0;
    std::string res;
    newPos++;
    res += executeSubRules(program, newPos, scope, internalState);
    if (!res.length())
        throw szl::SZLException("Right shift syntax error");
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("Right shift syntax error");
    if (internalState.back() != "uint")
        throw szl::SZLException("Second argument of right shift can only be of type uint");
    internalState.pop_back();

    // STRING
    if (internalState.back() == "string")
    {
        throw szl::SZLException("Strings can not be arguments of right shift");
    }

    // INT/UINT/CHAR
    if (internalState.back() == "int" || internalState.back() == "uint" || internalState.back() == "char")
    {
        return resL + "PUSH HL\n" + res + "POP DE\nEX DE,HL\nCALL @stdszllib_right_shift_16bit\n";
    }

    //  LONG/ULONG
    if (internalState.back() == "long" || internalState.back() == "ulong")
    {
        return resL + "PUSH HL\nPUSH DE\n" + res + "LD B,H\nLD C,L\nPOP DE\nPOP HL\nCALL @stdszllib_right_shift_32bit\n";
    }

    // FLOAT
    if (internalState.back() == "float")
    {
        throw szl::SZLException("Floats can not be arguments of right shift");
    }

    // BOOL
    if (internalState.back() == "bool")
    {
        throw szl::SZLException("Bools can not be arguments of right shift");
    }

    throw szl::SZLException("Right shift type unrecognized");
}

szl::GrammarShiftRight::GrammarShiftRight(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarShiftRight::initialize()
{
}

std::string szl::GrammarNotEqual::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::size_t newPos = position;
    std::string resL = executeSubRules(program, newPos, scope, internalState);
    if (!resL.length())
        return "";
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return returnWithClear(1, internalState);
    if (program[newPos].content != "!=")
        return returnWithClear(1, internalState);
    int num = 0;
    std::string res;
    newPos++;
    res += executeSubRules(program, newPos, scope, internalState);
    if (!res.length())
        throw szl::SZLException("Not equal comparison syntax error");
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("Not equal comparison syntax error");
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting not equal comparison of differing types");
    internalState.pop_back();

    // STRING
    if (internalState.back() == "string")
    {
        internalState.back() = "bool";
        // TODO
    }

    // INT/UINT/CHAR
    if (internalState.back() == "int" || internalState.back() == "uint" || internalState.back() == "char")
    {
        internalState.back() = "bool";
        return resL + "PUSH HL\n" + res + "POP DE\nCALL @stdszllib_not_equal_16bit\n";
    }

    //  LONG/ULONG
    if (internalState.back() == "long" || internalState.back() == "ulong")
    {
        internalState.back() = "bool";
        return resL + "PUSH HL\nPUSH DE\n" + res + "POP DE\nCALL @stdszllib_not_equal_32bit\n";
    }

    // FLOAT
    if (internalState.back() == "float")
    {
        internalState.back() = "bool";
        return resL + "PUSH HL\nPUSH DE\n" + res + "POP DE\nCALL @stdszllib_not_equal_float\n";
    }

    // BOOL
    if (internalState.back() == "bool")
    {
        return resL + "PUSH HL\n" + res + "POP DE\nCALL @stdszllib_xor_bool\n";
    }

    throw szl::SZLException("Not equal comparison type unrecognized");
}

szl::GrammarNotEqual::GrammarNotEqual(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarNotEqual::initialize()
{
}

std::string szl::GrammarEqual::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::size_t newPos = position;
    std::string resL = executeSubRules(program, newPos, scope, internalState);
    if (!resL.length())
        return "";
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return returnWithClear(1, internalState);
    if (program[newPos].content != "==")
        return returnWithClear(1, internalState);
    int num = 0;
    std::string res;
    newPos++;
    res += executeSubRules(program, newPos, scope, internalState);
    if (!res.length())
        throw szl::SZLException("Equal comparison syntax error");
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("Equal comparison syntax error");
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting equal comparison of differing types");
    internalState.pop_back();

    // STRING
    if (internalState.back() == "string")
    {
        internalState.back() = "bool";
        // TODO
    }

    // INT/UINT/CHAR
    if (internalState.back() == "int" || internalState.back() == "uint" || internalState.back() == "char")
    {
        internalState.back() = "bool";
        return resL + "PUSH HL\n" + res + "POP DE\nCALL @stdszllib_equal_16bit\n";
    }

    //  LONG/ULONG
    if (internalState.back() == "long" || internalState.back() == "ulong")
    {
        internalState.back() = "bool";
        return resL + "PUSH HL\nPUSH DE\n" + res + "POP DE\nCALL @stdszllib_equal_32bit\n";
    }

    // FLOAT
    if (internalState.back() == "float")
    {
        internalState.back() = "bool";
        return resL + "PUSH HL\nPUSH DE\n" + res + "POP DE\nCALL @stdszllib_equal_float\n";
    }

    // BOOL
    if (internalState.back() == "bool")
    {
        return resL + "PUSH HL\n" + res + "POP DE\nCALL @stdszllib_equal_bool\n";
    }

    throw szl::SZLException("Equal comparison type unrecognized");
}

szl::GrammarEqual::GrammarEqual(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarEqual::initialize()
{
}

std::string szl::GrammarGreater::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::size_t newPos = position;
    std::string resL = executeSubRules(program, newPos, scope, internalState);
    if (!resL.length())
        return "";
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return returnWithClear(1, internalState);
    if (program[newPos].content != ">")
        return returnWithClear(1, internalState);
    int num = 0;
    std::string res;
    newPos++;
    res += executeSubRules(program, newPos, scope, internalState);
    if (!res.length())
        throw szl::SZLException("Greater comparison syntax error");
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("Greater comparison syntax error");
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting greater comparison of differing types");
    internalState.pop_back();

    // STRING
    if (internalState.back() == "string")
    {
        throw szl::SZLException("Strings can not be arguments of greater comparison");
    }

    // UINT/CHAR
    if (internalState.back() == "uint" || internalState.back() == "char")
    {
        internalState.back() = "bool";
        return resL + "PUSH HL\n" + res + "POP DE\nCALL @stdszllib_less_uint\n";
    }

    // INT
    if (internalState.back() == "int")
    {
        internalState.back() = "bool";
        return resL + "PUSH HL\n" + res + "POP DE\nCALL @stdszllib_less_int\n";
    }

    //  ULONG
    if (internalState.back() == "ulong")
    {
        internalState.back() = "bool";
        return resL + "PUSH HL\nPUSH DE\n" + res + "EXX\nPOP BC\nEXX\nPOP BC\nCALL @stdszllib_less_ulong\n";
    }

    //  LONG
    if (internalState.back() == "long")
    {
        internalState.back() = "bool";
        return resL + "PUSH HL\nPUSH DE\n" + res + "EXX\nPOP BC\nEXX\nPOP BC\nCALL @stdszllib_less_long\n";
    }

    // FLOAT
    if (internalState.back() == "float")
    {
        internalState.back() = "bool";
        return resL + "PUSH HL\nPUSH DE\n" + res + "EXX\nPOP BC\nEXX\nPOP BC\nCALL @stdszllib_less_float\n";
    }

    // BOOL
    if (internalState.back() == "bool")
    {
        throw szl::SZLException("Boolean values can not be arguments of greater comparison");
    }

    throw szl::SZLException("Greater comparison type unrecognized");
}

szl::GrammarGreater::GrammarGreater(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarGreater::initialize()
{
}

std::string szl::GrammarLess::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::size_t newPos = position;
    std::string resL = executeSubRules(program, newPos, scope, internalState);
    if (!resL.length())
        return "";
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return returnWithClear(1, internalState);
    if (program[newPos].content != "<")
        return returnWithClear(1, internalState);
    int num = 0;
    std::string res;
    newPos++;
    res += executeSubRules(program, newPos, scope, internalState);
    if (!res.length())
        throw szl::SZLException("Less comparison syntax error");
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("Less comparison syntax error");
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting less comparison of differing types");
    internalState.pop_back();

    // STRING
    if (internalState.back() == "string")
    {
        throw szl::SZLException("Strings can not be arguments of less comparison");
    }

    // UINT/CHAR
    if (internalState.back() == "uint" || internalState.back() == "char")
    {
        internalState.back() = "bool";
        return resL + "PUSH HL\n" + res + "POP DE\nCALL @stdszllib_greater_uint\n";
    }

    // INT
    if (internalState.back() == "int")
    {
        internalState.back() = "bool";
        return resL + "PUSH HL\n" + res + "POP DE\nCALL @stdszllib_greater_int\n";
    }

    //  ULONG
    if (internalState.back() == "ulong")
    {
        internalState.back() = "bool";
        return resL + "PUSH HL\nPUSH DE\n" + res + "EXX\nPOP BC\nEXX\nPOP BC\nCALL @stdszllib_greater_ulong\n";
    }

    //  LONG
    if (internalState.back() == "long")
    {
        internalState.back() = "bool";
        return resL + "PUSH HL\nPUSH DE\n" + res + "EXX\nPOP BC\nEXX\nPOP BC\nCALL @stdszllib_greater_long\n";
    }

    // FLOAT
    if (internalState.back() == "float")
    {
        internalState.back() = "bool";
        return resL + "PUSH HL\nPUSH DE\n" + res + "EXX\nPOP BC\nEXX\nPOP BC\nCALL @stdszllib_greater_float\n";
    }

    // BOOL
    if (internalState.back() == "bool")
    {
        throw szl::SZLException("Boolean values can not be arguments of less comparison");
    }

    throw szl::SZLException("Less comparison type unrecognized");
}

szl::GrammarLess::GrammarLess(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarLess::initialize()
{
    addSubRule("identifier");
    addSubRule("literal");
}

std::string szl::GrammarGreaterOrEqual::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::size_t newPos = position;
    std::string resL = executeSubRules(program, newPos, scope, internalState);
    if (!resL.length())
        return "";
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return returnWithClear(1, internalState);
    if (program[newPos].content != ">=")
        return returnWithClear(1, internalState);
    int num = 0;
    std::string res;
    newPos++;
    res += executeSubRules(program, newPos, scope, internalState);
    if (!res.length())
        throw szl::SZLException("Greater or equal comparison syntax error");
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("Greater or equal comparison syntax error");
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting greater or equal comparison of differing types");
    internalState.pop_back();

    // STRING
    if (internalState.back() == "string")
    {
        throw szl::SZLException("Strings can not be arguments of greater or equal comparison");
    }

    // UINT/CHAR
    if (internalState.back() == "uint" || internalState.back() == "char")
    {
        internalState.back() = "bool";
        return resL + "PUSH HL\n" + res + "POP DE\nCALL @stdszllib_less_or_equal_uint\n";
    }

    // INT
    if (internalState.back() == "int")
    {
        internalState.back() = "bool";
        return resL + "PUSH HL\n" + res + "POP DE\nCALL @stdszllib_less_or_equal_int\n";
    }

    //  ULONG
    if (internalState.back() == "ulong")
    {
        internalState.back() = "bool";
        return resL + "PUSH HL\nPUSH DE\n" + res + "EXX\nPOP BC\nEXX\nPOP BC\nCALL @stdszllib_less_or_equal_ulong\n";
    }

    //  LONG
    if (internalState.back() == "long")
    {
        internalState.back() = "bool";
        return resL + "PUSH HL\nPUSH DE\n" + res + "EXX\nPOP BC\nEXX\nPOP BC\nCALL @stdszllib_less_or_equal_long\n";
    }

    // FLOAT
    if (internalState.back() == "float")
    {
        internalState.back() = "bool";
        return resL + "PUSH HL\nPUSH DE\n" + res + "EXX\nPOP BC\nEXX\nPOP BC\nCALL @stdszllib_less_or_equal_float\n";
    }

    // BOOL
    if (internalState.back() == "bool")
    {
        throw szl::SZLException("Boolean values can not be arguments of greater or equal comparison");
    }

    throw szl::SZLException("Greater or equal comparison type unrecognized");
}

szl::GrammarGreaterOrEqual::GrammarGreaterOrEqual(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarGreaterOrEqual::initialize()
{
}

std::string szl::GrammarLessOrEqual::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::size_t newPos = position;
    std::string resL = executeSubRules(program, newPos, scope, internalState);
    if (!resL.length())
        return "";
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return returnWithClear(1, internalState);
    if (program[newPos].content != "<=")
        return returnWithClear(1, internalState);
    int num = 0;
    std::string res;
    newPos++;
    res += executeSubRules(program, newPos, scope, internalState);
    if (!res.length())
        throw szl::SZLException("Less or equal comparison syntax error");
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("Less or equal comparison syntax error");
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting less or equal comparison of differing types");
    internalState.pop_back();

    // STRING
    if (internalState.back() == "string")
    {
        throw szl::SZLException("Strings can not be arguments of less or equal comparison");
    }

    // UINT/CHAR
    if (internalState.back() == "uint" || internalState.back() == "char")
    {
        internalState.back() = "bool";
        return resL + "PUSH HL\n" + res + "POP DE\nCALL @stdszllib_greater_or_equal_uint\n";
    }

    // INT
    if (internalState.back() == "int")
    {
        internalState.back() = "bool";
        return resL + "PUSH HL\n" + res + "POP DE\nCALL @stdszllib_greater_or_equal_int\n";
    }

    //  ULONG
    if (internalState.back() == "ulong")
    {
        internalState.back() = "bool";
        return resL + "PUSH HL\nPUSH DE\n" + res + "EXX\nPOP BC\nEXX\nPOP BC\nCALL @stdszllib_greater_or_equal_ulong\n";
    }

    //  LONG
    if (internalState.back() == "long")
    {
        internalState.back() = "bool";
        return resL + "PUSH HL\nPUSH DE\n" + res + "EXX\nPOP BC\nEXX\nPOP BC\nCALL @stdszllib_greater_or_equal_long\n";
    }

    // FLOAT
    if (internalState.back() == "float")
    {
        internalState.back() = "bool";
        return resL + "PUSH HL\nPUSH DE\n" + res + "EXX\nPOP BC\nEXX\nPOP BC\nCALL @stdszllib_greater_or_equal_float\n";
    }

    // BOOL
    if (internalState.back() == "bool")
    {
        throw szl::SZLException("Boolean values can not be arguments of less or equal comparison");
    }

    throw szl::SZLException("Less or equal comparison type unrecognized");
}

szl::GrammarLessOrEqual::GrammarLessOrEqual(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarLessOrEqual::initialize()
{
}

std::string szl::GrammarIf::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::size_t newPos = position;
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return "";
    if (program[newPos].content != "if")
        return "";
    newPos++;
    if (newPos >= program.size())
        return "";
    if (program[newPos].category != szl::TokenCategory::Bracket)
        return "";
    if (program[newPos].content != "(")
        return "";
    newPos++;
    std::string res = executeSubRules(program, newPos, scope, internalState);
    if (!res.length())
        throw szl::SZLException("If statement syntax error");
    if (program[newPos].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("If statement syntax error");
    if (program[newPos].content != ")")
        throw szl::SZLException("If statement syntax error");
    position = newPos + 1;
    if (position >= program.size())
        throw szl::SZLException("If statement syntax error: EOF");
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("If statement syntax error");
    if (program[position].content != "{")
        throw szl::SZLException("If statement syntax error");
    if (internalState.size() < 1)
        throw szl::SZLException("If statement requires condition");
    if (internalState.back() != "bool")
        throw szl::SZLException("If statement requires boolean condition");
    internalState.pop_back();

    res += "LD DE,%0\nOR A\nSBC HL,DE\nJP Z," + scope.back().getNextLabel() + "\n";
    scope.emplace_back(0, &res, &scope.back());
    position++;
    res += compileScope(program, position, scope, internalState);
    scope.back().changeCode(&res);
    scope.pop_back();
    return res;
}

szl::GrammarIf::GrammarIf(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarIf::initialize()
{
    addSubRule("two literals addition");
    addSubRule("brackets");
    addSubRule("identifier");
    addSubRule("literal");
    addSubRule("addition");
    addSubRule("subtraction");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("and");
    addSubRule("or");
    addSubRule("xor");
    addSubRule("modulo");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("shift left");
    addSubRule("shift right");
    addSubRule("not equal");
    addSubRule("equal");
    addSubRule("greater");
    addSubRule("greater or equal");
    addSubRule("less");
    addSubRule("less or equal");
}

std::string szl::GrammarScopeClosed::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::size_t newPos = position;
    if (program[position].category != szl::TokenCategory::Bracket)
        return "";
    if (program[position].content != "}")
        return "";
    position++;
    return ";Close scope\n";
}

szl::GrammarScopeClosed::GrammarScopeClosed(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarScopeClosed::initialize()
{
}

std::string szl::GrammarWhile::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::size_t newPos = position;
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return "";
    if (program[newPos].content != "while")
        return "";
    newPos++;
    if (newPos >= program.size())
        return "";
    if (program[newPos].category != szl::TokenCategory::Bracket)
        return "";
    if (program[newPos].content != "(")
        return "";
    newPos++;
    auto id = std::to_string(szl::nextUniqueId++);
    std::string res = "@szlCompilerLabelWhileLoopId" + id + "\n" + executeSubRules(program, newPos, scope, internalState);
    if (!res.length())
        throw szl::SZLException("While statement syntax error");
    if (program[newPos].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("While statement syntax error");
    if (program[newPos].content != ")")
        throw szl::SZLException("While statement syntax error");
    position = newPos + 1;
    if (position >= program.size())
        throw szl::SZLException("While statement syntax error: EOF");
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("While statement syntax error");
    if (program[position].content != "{")
        throw szl::SZLException("While statement syntax error");
    if (internalState.size() < 1)
        throw szl::SZLException("While statement requires condition");
    if (internalState.back() != "bool")
        throw szl::SZLException("While statement requires boolean condition");
    internalState.pop_back();

    res += "LD DE,%0\nOR A\nSBC HL,DE\nJP Z," + scope.back().getNextLabel() + "\n";
    scope.emplace_back(0, &res, &scope.back());
    position++;
    res += compileScope(program, position, scope, internalState);
    scope.back().changeCode(&res);
    scope.back().addCustomDeleteCode("JP @szlCompilerLabelWhileLoopId" + id + "\n");
    scope.pop_back();
    return res;
}

szl::GrammarWhile::GrammarWhile(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarWhile::initialize()
{
    addSubRule("two literals addition");
    addSubRule("brackets");
    addSubRule("identifier");
    addSubRule("literal");
    addSubRule("addition");
    addSubRule("subtraction");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("and");
    addSubRule("or");
    addSubRule("xor");
    addSubRule("modulo");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("shift left");
    addSubRule("shift right");
    addSubRule("not equal");
    addSubRule("equal");
    addSubRule("greater");
    addSubRule("greater or equal");
    addSubRule("less");
    addSubRule("less or equal");
}

std::string szl::GrammarFor::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::size_t newPos = position;
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return "";
    if (program[newPos].content != "for")
        return "";
    newPos++;
    if (newPos >= program.size())
        return "";
    if (program[newPos].category != szl::TokenCategory::Bracket)
        return "";
    if (program[newPos].content != "(")
        return "";
    newPos++;
    auto id = std::to_string(szl::nextUniqueId++);
    std::string res;
    scope.emplace_back(12, &res, &scope.back());
    res += executeSubRules(program, newPos, scope, internalState) + "@szlCompilerLabelForLoopId" + id + "\n";
    if (!res.length())
        throw szl::SZLException("For statement syntax error");
    // if (program[newPos].category != szl::TokenCategory::Punctuation)
    //     throw szl::SZLException("For statement syntax error");
    // if (program[newPos].content != ";")
    //     throw szl::SZLException("For statement syntax error");
    // position = newPos + 1;
    position = newPos;

    res += executeSubRules(program, position, scope, internalState);
    if (!res.length())
        throw szl::SZLException("For statement syntax error");
    if (program[position].category != szl::TokenCategory::Punctuation)
        throw szl::SZLException("For statement syntax error");
    if (program[position].content != ";")
        throw szl::SZLException("For statement syntax error");
    position++;
    if (position >= program.size())
        throw szl::SZLException("While statement syntax error: EOF");

    auto internalBuffer = internalState;
    std::string endingCode = executeSubRules(program, position, scope, internalState);
    if (!res.length())
        throw szl::SZLException("For statement syntax error");
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("For statement syntax error");
    if (program[position].content != ")")
        throw szl::SZLException("For statement syntax error");
    position++;
    internalState = internalBuffer;

    if (position >= program.size())
        throw szl::SZLException("For statement syntax error: EOF");
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("For statement syntax error");
    if (program[position].content != "{")
        throw szl::SZLException("For statement syntax error");
    if (internalState.size() < 1)
        throw szl::SZLException("For statement requires condition");
    if (internalState.back() != "bool")
        throw szl::SZLException("For statement requires boolean condition");
    internalState.pop_back();

    res += "LD DE,%0\nOR A\nSBC HL,DE\nJP Z," + scope.back().getNextLabel() + "\n";
    scope.emplace_back(0, &res, &scope.back());
    position++;
    res += compileScope(program, position, scope, internalState);
    scope.back().changeCode(&res);
    scope.back().addCustomDeleteCode(endingCode + "JP @szlCompilerLabelForLoopId" + id + "\n");
    scope.pop_back();
    scope.back().changeCode(&res);
    scope.pop_back();
    return res;
}

szl::GrammarFor::GrammarFor(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarFor::initialize()
{
    addSubRule("not equal");
    addSubRule("equal");
    addSubRule("greater");
    addSubRule("greater or equal");
    addSubRule("less");
    addSubRule("less or equal");
    addSubRule("addition");
    addSubRule("subtraction");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("and");
    addSubRule("or");
    addSubRule("xor");
    addSubRule("modulo");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("shift left");
    addSubRule("shift right");
    addSubRule("assignment");
    addSubRule("single variable declaration");
    addSubRule("two literals addition");
    addSubRule("brackets");
    addSubRule("identifier");
    addSubRule("literal");
}

szl::Function szl::GrammarFunctionDeclaration::createFunctionTableEntry(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::vector<std::pair<std::string, std::string>> arguments;
    if (program[position].category != szl::TokenCategory::Keyword)
        throw szl::SZLException("Syntax error while declaring new function");
    auto returns = program[position++].content;
    if (position >= program.size())
        throw szl::SZLException("Unexpected EOF while declaring new function");
    if (program[position].category != szl::TokenCategory::Identifier)
        throw szl::SZLException("Syntax error while declaring new function");
    auto name = program[position++].content;
    if (szl::functions.count(name) > 0)
        throw szl::SZLException("Redeclaration of function '" + name + "'");
    for (const auto &it : scope)
    {
        if (it.exists(name))
            throw szl::SZLException("Redeclaration of variable '" + name + "'");
    }
    if (position >= program.size())
        throw szl::SZLException("Unexpected EOF while declaring new function");
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("Syntax error while declaring new function");
    if (program[position++].content != "(")
        throw szl::SZLException("Syntax error while declaring new function");

    while (program[position].category == szl::TokenCategory::Keyword)
    {
        auto type = program[position].content;
        if (position++ >= program.size())
            throw szl::SZLException("Unexpected EOF while declaring new function");
        if (program[position].category != szl::TokenCategory::Identifier)
            throw szl::SZLException("Syntax error while declaring new function");
        if (type != "int" && type != "uint" && type != "char" && type != "bool" && type != "long" && type != "ulong" && type != "float" && type != "string")
            throw szl::SZLException("Argument type unrecognized while declaring new function");
        arguments.push_back({type, program[position++].content});
        if (position >= program.size())
            throw szl::SZLException("Unexpected EOF while declaring new function");
        if (program[position].category != szl::TokenCategory::Punctuation && program[position].category != szl::TokenCategory::Bracket)
            throw szl::SZLException("Syntax error while declaring new function");
        if (program[position].content != "," && program[position].content != ")")
            throw szl::SZLException("Syntax error while declaring new function");
        position++;
    }
    if (arguments.size() > 0)
        position--;
    if (position >= program.size())
        throw szl::SZLException("Unexpected EOF while declaring new function");
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("Syntax error while declaring new function");
    if (program[position++].content != ")")
        throw szl::SZLException("Syntax error while declaring new function");
    if (position >= program.size())
        throw szl::SZLException("Unexpected EOF while declaring new function");
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("Syntax error while declaring new function");
    if (program[position++].content != "{")
        throw szl::SZLException("Syntax error while declaring new function");
    szl::functions.emplace(name, szl::Function(name, arguments, returns));
    return szl::functions[name];
}

std::string szl::GrammarFunctionDeclaration::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    if (position + 4 >= program.size())
        return "";
    if (program[position].category != szl::TokenCategory::Keyword)
        return "";
    if (program[position + 1].category != szl::TokenCategory::Identifier)
        return "";
    if (program[position + 2].category != szl::TokenCategory::Bracket)
        return "";

    auto type = program[position].content;
    if (type != "int" && type != "uint" && type != "long" && type != "ulong" && type != "bool" && type != "float" && type != "char" && type != "string" && type != "void")
        return "";
    if (program[position + 2].content != "(")
        return "";
    if (scope.size() > 1)
        throw szl::SZLException("Functions can not be defined inside existing scopes");

    auto function = createFunctionTableEntry(program, position, scope, internalState);
    std::string res = function.getLabel();
    int retSize = 0;

    if (type == "int" || type == "uint" || type == "char" || type == "bool" || type == "string")
        retSize = 2;
    else if (type == "long" || type == "ulong" || type == "float")
        retSize = 4;
    else if (type == "void")
        retSize = 0;
    else
        throw szl::SZLException("Function return type not defined");

    auto arguments = function.getArguments();
    std::list<szl::Scope> newScope;
    newScope.emplace_back(12, &res, &scope.front());
    for (std::size_t i = 0; i < arguments.size(); i++)
    {
        int sz = 0;
        auto t = arguments[i].first;
        if (t == "int" || t == "uint" || t == "char" || t == "bool" || t == "string")
        {
            sz = 2;
        }
        else if (t == "long" || t == "ulong" || t == "float")
        {
            sz = 4;
        }
        else
            throw szl::SZLException("Function argument type unknown");
        newScope.back().insertVariable(arguments[i].second, sz, t);
    }
    newScope.emplace_back(retSize, &res, &newScope.back());
    res += compileScope(program, position, newScope, internalState);
    newScope.back().changeCode(&res);
    newScope.pop_back();
    newScope.back().changeCode(&res);
    newScope.pop_back();
    return res + "RET\n";
}

szl::GrammarFunctionDeclaration::GrammarFunctionDeclaration(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarFunctionDeclaration::initialize()
{
}

std::string szl::GrammarReturn::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    if (program[position].category != szl::TokenCategory::Keyword)
        return "";
    if (program[position].content != "return")
        return "";
    if (++position >= program.size())
        throw szl::SZLException("Unexpected EOF on return");
    return executeSubRules(program, position, scope, internalState);
}

szl::GrammarReturn::GrammarReturn(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarReturn::initialize()
{
    addSubRule("not equal");
    addSubRule("equal");
    addSubRule("greater");
    addSubRule("greater or equal");
    addSubRule("less");
    addSubRule("less or equal");
    addSubRule("addition");
    addSubRule("subtraction");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("and");
    addSubRule("or");
    addSubRule("xor");
    addSubRule("modulo");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("shift left");
    addSubRule("shift right");
    addSubRule("assignment");
    addSubRule("single variable declaration");
    addSubRule("two literals addition");
    addSubRule("brackets");
    addSubRule("identifier");
    addSubRule("literal");
    addSubRule("at");
    addSubRule("semicolon");
}

szl::Function szl::GrammarFunctionCall::getFunctionTableEntry(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    if (program[position].category != szl::TokenCategory::Identifier)
        throw szl::SZLException("Syntax error while calling function");
    auto name = program[position++].content;
    if (position >= program.size())
        throw szl::SZLException("Unexpected EOF while calling function");
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("Syntax error while calling function");
    if (program[position++].content != "(")
        throw szl::SZLException("Syntax error while calling function");
    if (szl::functions.count(name) < 1)
        throw szl::SZLException("Function with name '" + name + "' does not exist");
    return szl::functions[name];
}

std::string szl::GrammarFunctionCall::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    if (position + 1 >= program.size())
        return "";
    if (program[position].category != szl::TokenCategory::Identifier)
        return "";
    if (program[position + 1].category != szl::TokenCategory::Bracket)
        return "";
    if (program[position + 1].content != "(")
        return "";
    auto function = getFunctionTableEntry(program, position, scope, internalState);
    auto expectedArguments = function.getArguments();
    std::string res;
    for (std::size_t i = 0; i < expectedArguments.size(); i++)
    {
        auto type = expectedArguments[i].first;
        std::string subRes = executeSubRules(program, position, scope, internalState);
        if (!subRes.length())
            throw szl::SZLException("Expression inside function call does not evaluate to any known value type");
        if (!internalState.size())
            throw szl::SZLException("Expression inside function call does not evaluate to any known value type");
        if (internalState.back() != type)
            throw szl::SZLException("Argument mismatch inside function call: expected " + type + ", got " + internalState.back() + "instead");
        if (type == "int" || type == "uint" || type == "char" || type == "bool" || type == "string")
            subRes += "PUSH HL\n";
        else if (type == "long" || type == "ulong" || type == "float")
            subRes += "PUSH HL\nPUSH DE\n";
        else
            throw szl::SZLException("Internal compiler error: function table defines argument type as '" + type + "', but this type is not known");
        if (program[position].category != szl::TokenCategory::Punctuation && program[position].category != szl::TokenCategory::Bracket)
            throw szl::SZLException("Syntax error while calling function");
        if (program[position].content != "," && program[position].content != ")")
            throw szl::SZLException("Syntax error while calling function");
        position++;
        res += subRes;
    }
    return res + "JP " + function.getLabel();
}

szl::GrammarFunctionCall::GrammarFunctionCall(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarFunctionCall::initialize()
{
    addSubRule("not equal");
    addSubRule("equal");
    addSubRule("greater");
    addSubRule("greater or equal");
    addSubRule("less");
    addSubRule("less or equal");
    addSubRule("addition");
    addSubRule("subtraction");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("and");
    addSubRule("or");
    addSubRule("xor");
    addSubRule("modulo");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("shift left");
    addSubRule("shift right");
    addSubRule("assignment");
    addSubRule("single variable declaration");
    addSubRule("two literals addition");
    addSubRule("brackets");
    addSubRule("identifier");
    addSubRule("literal");
}

std::string szl::GrammarLock::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    if (program[position].category != szl::TokenCategory::Keyword)
        return "";
    if (program[position].content != "lock();")
        return "";
    position++;
    return "DI\n";
}

szl::GrammarLock::GrammarLock(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarLock::initialize() {}

std::string szl::GrammarUnlock::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    if (program[position].category != szl::TokenCategory::Keyword)
        return "";
    if (program[position].content != "unlock();")
        return "";
    position++;
    return "EI\n";
}

szl::GrammarUnlock::GrammarUnlock(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarUnlock::initialize() {}

std::string szl::GrammarIn::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    if (program[position].category != szl::TokenCategory::Keyword)
        return "";
    if (program[position].content != "in")
        return "";
    if (program.size() <= ++position)
        throw szl::SZLException("Unexpected EOF while calling 'in'");
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("Syntax error while calling 'in'");
    if (program[position++].content != "(")
        throw szl::SZLException("Syntax error while calling 'in'");
    std::string res = executeSubRules(program, position, scope, internalState);
    if (!internalState.size())
        throw szl::SZLException("No valid address specified while calling 'in'");
    if (internalState.back() != "uint")
        throw szl::SZLException("Address is not of type uint while calling 'in'");
    if (program.size() <= position)
        throw szl::SZLException("Unexpected EOF while calling 'in'");
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("Syntax error while calling 'in'");
    if (program[position++].content != ")")
        throw szl::SZLException("Syntax error while calling 'in'");
    internalState.back() = "int";
    return res + "LD C,L\nIN L,(C)\nLD H,%0\n";
}

szl::GrammarIn::GrammarIn(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarIn::initialize()
{
    addSubRule("two literals addition");
    addSubRule("brackets");
    addSubRule("addition");
    addSubRule("subtraction");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("and");
    addSubRule("or");
    addSubRule("xor");
    addSubRule("modulo");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("shift left");
    addSubRule("shift right");
    addSubRule("not equal");
    addSubRule("equal");
    addSubRule("greater");
    addSubRule("greater or equal");
    addSubRule("less");
    addSubRule("less or equal");
    addSubRule("identifier");
    addSubRule("literal");
    addSubRule("in");
    addSubRule("function call");
}

std::string szl::GrammarOut::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    if (program[position].category != szl::TokenCategory::Keyword)
        return "";
    if (program[position].content != "out")
        return "";
    if (program.size() <= ++position)
        throw szl::SZLException("Unexpected EOF while calling 'out'");
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("Syntax error while calling 'out'");
    if (program[position++].content != "(")
        throw szl::SZLException("Syntax error while calling 'out'");
    std::string res = executeSubRules(program, position, scope, internalState) + "PUSH HL\n";
    if (!internalState.size())
        throw szl::SZLException("No valid address specified while calling 'out'");
    if (internalState.back() != "uint")
        throw szl::SZLException("Address is not of type uint while calling 'out'");
    if (program.size() <= position)
        throw szl::SZLException("Unexpected EOF while calling 'out'");
    if (program[position].category != szl::TokenCategory::Punctuation)
        throw szl::SZLException("Syntax error while calling 'out'");
    if (program[position++].content != ",")
        throw szl::SZLException("Syntax error while calling 'out'");
    internalState.pop_back();
    res += executeSubRules(program, position, scope, internalState);
    if (!internalState.size())
        throw szl::SZLException("No valid data specified while calling 'out'");
    if (internalState.back() != "int")
        throw szl::SZLException("Data is not of type int while calling 'out'");
    if (program.size() <= position)
        throw szl::SZLException("Unexpected EOF while calling 'out'");
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("Syntax error while calling 'out'");
    if (program[position++].content != ")")
        throw szl::SZLException("Syntax error while calling 'out'");
    internalState.pop_back();
    return res + "POP BC\nOUT (C),L\n";
}

szl::GrammarOut::GrammarOut(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarOut::initialize()
{
    addSubRule("two literals addition");
    addSubRule("brackets");
    addSubRule("addition");
    addSubRule("subtraction");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("and");
    addSubRule("or");
    addSubRule("xor");
    addSubRule("modulo");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("shift left");
    addSubRule("shift right");
    addSubRule("not equal");
    addSubRule("equal");
    addSubRule("greater");
    addSubRule("greater or equal");
    addSubRule("less");
    addSubRule("less or equal");
    addSubRule("identifier");
    addSubRule("literal");
    addSubRule("in");
    addSubRule("function call");
}

std::string szl::GrammarAt::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::size_t newPos = position;
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return "";
    auto type = program[newPos++].content;

    if (program.size() <= newPos)
        return "";
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return "";
    if (program[newPos++].content != "@")
        return "";
    std::string res = executeSubRules(program, position = newPos, scope, internalState);
    if (!res.length())
        throw szl::SZLException("@ syntax error");
    if (internalState.size() < 1)
        throw szl::SZLException("@ requires memory address as right operand");
    if (internalState.back() != "uint")
        throw szl::SZLException("@ expects uint as right operand");
    internalState.back() = type;

    // STRING
    if (internalState.back() == "string")
    {
        // TODO
    }

    // INT/UINT/CHAR/BOOL
    if (internalState.back() == "int" || internalState.back() == "uint" || internalState.back() == "char" || internalState.back() == "bool")
    {
        return res + "LD E,(HL)\nDEC HL\nLD D,(HL)\nEX DE,HL\n";
    }

    //  ULONG/LONG/FLOAT
    if (internalState.back() == "ulong" || internalState.back() == "long" || internalState.back() == "float")
    {
        return res + "LD E,(HL)\nDEC HL\nLD D,(HL)\nDEC HL\nLD C,(HL)\nDEC HL\nLD H,(HL)\nLD L,C\n";
    }

    // TODO : other types
    throw szl::SZLException("@ type not recognized");
}

szl::GrammarAt::GrammarAt(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarAt::initialize()
{
    addSubRule("two literals addition");
    addSubRule("brackets");
    addSubRule("addition");
    addSubRule("subtraction");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("and");
    addSubRule("or");
    addSubRule("xor");
    addSubRule("modulo");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("shift left");
    addSubRule("shift right");
    addSubRule("not equal");
    addSubRule("equal");
    addSubRule("greater");
    addSubRule("greater or equal");
    addSubRule("less");
    addSubRule("less or equal");
    addSubRule("identifier");
    addSubRule("literal");
    addSubRule("in");
    addSubRule("function call");
}

std::string szl::GrammarArrow::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::size_t newPos = position;
    std::string resL = executeSubRules(program, newPos, scope, internalState);
    if (!resL.length())
        return "";
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return returnWithClear(1, internalState);
    if (program[newPos++].content != "->")
        return returnWithClear(1, internalState);
    std::string res = executeSubRules(program, position = newPos, scope, internalState);
    if (!res.length())
        throw szl::SZLException("-> syntax error");
    if (internalState.size() < 2)
        throw szl::SZLException("-> requires two operands");
    if (internalState.back() != "uint")
        throw szl::SZLException("-> expects uint as right operand");
    internalState.pop_back();

    // STRING
    if (internalState.back() == "string")
    {
        // TODO
    }

    // INT/UINT/CHAR/BOOL
    if (internalState.back() == "int" || internalState.back() == "uint" || internalState.back() == "char" || internalState.back() == "bool")
    {
        return resL + "PUSH HL\n" + res + "POP DE\nLD (HL),E\nDEC HL\nLD (HL),D\n";
    }

    //  ULONG/LONG/FLOAT
    if (internalState.back() == "ulong" || internalState.back() == "long" || internalState.back() == "float")
    {
        return resL + "PUSH HL\nPUSH DE\n" + res + "POP DE\nLD (HL),E\nDEC HL\nLD (HL),D\nPOP DE\nDEC HL\nLD (HL),E\nDEC HL\nLD (HL),D\n";
    }

    // TODO : other types
    throw szl::SZLException("-> type not recognized");
}

szl::GrammarArrow::GrammarArrow(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarArrow::initialize()
{
    addSubRule("questioned at");
    addSubRule("two literals addition");
    addSubRule("brackets");
    addSubRule("addition");
    addSubRule("subtraction");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("and");
    addSubRule("or");
    addSubRule("xor");
    addSubRule("modulo");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("shift left");
    addSubRule("shift right");
    addSubRule("not equal");
    addSubRule("equal");
    addSubRule("greater");
    addSubRule("greater or equal");
    addSubRule("less");
    addSubRule("less or equal");
    addSubRule("identifier");
    addSubRule("literal");
    addSubRule("in");
    addSubRule("function call");
}

std::string szl::GrammarQuestionedAt::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    auto newPos = position;
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return "";
    if (program[newPos].content != "?")
        return "";
    if (++newPos >= program.size())
        return "";
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return "";
    if (program[newPos].content != "@")
        return "";
    if (++newPos >= program.size())
        throw szl::SZLException("Unexpected EOF on ?@");
    if (program[position = newPos].category != szl::TokenCategory::Identifier)
        throw szl::SZLException("Syntax error on ?@ functionality");
    if (!scope.back().exists(program[position].content))
        throw szl::SZLException("Requested address of variable '" + program[position].content + "', which does not exist");
    internalState.push_back("uint");
    return "LD HL,#" + std::to_string(scope.back()[program[position++].content].getPosition()) + "\n";
}

szl::GrammarQuestionedAt::GrammarQuestionedAt(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarQuestionedAt::initialize()
{
}

std::string szl::GrammarAlloc::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    auto newPos = position;
    std::string res;
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return "";
    if (program[newPos].content != "alloc")
        return "";
    if (++newPos >= program.size())
        return "";
    if (program[newPos].category != szl::TokenCategory::Bracket)
        return "";
    if (program[newPos].content != "(")
        return "";
    if (++newPos >= program.size())
        throw szl::SZLException("Unexpected EOF on alloc");
    res = executeSubRules(program, position = newPos, scope, internalState);
    if (!res.length())
        throw szl::SZLException("Alloc expects one parameter");
    if (internalState.size() < 1)
        throw szl::SZLException("Alloc expects one parameter");
    if (internalState.back() != "uint")
        throw szl::SZLException("Alloc expects uint parameter");
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("Syntax error on alloc");
    if (program[position++].content != ")")
        throw szl::SZLException("Syntax error on alloc");
    return res + "LD DE," + szl::programData["heap address"] + "\nCALL @stdszllib_alloc\n";
}

szl::GrammarAlloc::GrammarAlloc(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarAlloc::initialize()
{
    addSubRule("questioned at");
    addSubRule("two literals addition");
    addSubRule("brackets");
    addSubRule("addition");
    addSubRule("subtraction");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("and");
    addSubRule("or");
    addSubRule("xor");
    addSubRule("modulo");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("shift left");
    addSubRule("shift right");
    addSubRule("not equal");
    addSubRule("equal");
    addSubRule("greater");
    addSubRule("greater or equal");
    addSubRule("less");
    addSubRule("less or equal");
    addSubRule("identifier");
    addSubRule("literal");
    addSubRule("in");
    addSubRule("function call");
}

std::string szl::GrammarFree::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    auto newPos = position;
    std::string res, resL;
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return "";
    if (program[newPos].content != "free")
        return "";
    if (++newPos >= program.size())
        return "";
    if (program[newPos].category != szl::TokenCategory::Bracket)
        return "";
    if (program[newPos].content != "(")
        return "";
    if (++newPos >= program.size())
        throw szl::SZLException("Unexpected EOF on free");
    resL = executeSubRules(program, position = newPos, scope, internalState);
    if (!resL.length())
        throw szl::SZLException("Free expects two parameters");
    if (internalState.size() < 1)
        throw szl::SZLException("Alloc expects two parameters");
    if (internalState.back() != "uint")
        throw szl::SZLException("Alloc expects two uint parameters");
    if (position >= program.size())
        throw szl::SZLException("Unexpected EOF on free");
    if (program[position].category != szl::TokenCategory::Punctuation)
        throw szl::SZLException("Syntax error on free");
    if (program[position].content != ",")
        throw szl::SZLException("Syntax error on free");
    if (++position >= program.size())
        throw szl::SZLException("Unexpected EOF on free");
    res = executeSubRules(program, position, scope, internalState);
    if (!res.length())
        throw szl::SZLException("Free expects two parameters");
    if (internalState.size() < 2)
        throw szl::SZLException("Alloc expects two parameters");
    if (internalState.back() != "uint")
        throw szl::SZLException("Alloc expects two uint parameters");
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("Syntax error on alloc");
    if (program[position++].content != ")")
        throw szl::SZLException("Syntax error on alloc");
    internalState.pop_back();
    internalState.pop_back();
    return resL + "PUSH HL\n" + res + "POP DE\nEX DE,HL\nCALL @stdszllib_free\n";
}

szl::GrammarFree::GrammarFree(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarFree::initialize()
{
    addSubRule("questioned at");
    addSubRule("two literals addition");
    addSubRule("brackets");
    addSubRule("addition");
    addSubRule("subtraction");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("and");
    addSubRule("or");
    addSubRule("xor");
    addSubRule("modulo");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("shift left");
    addSubRule("shift right");
    addSubRule("not equal");
    addSubRule("equal");
    addSubRule("greater");
    addSubRule("greater or equal");
    addSubRule("less");
    addSubRule("less or equal");
    addSubRule("identifier");
    addSubRule("literal");
    addSubRule("in");
    addSubRule("function call");
}