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

std::string szl::Grammar::toBinFloat(double in, const std::string &file, const std::string &line)
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
        throw szl::SZLException("Error on float conversion: exponent too small", file, line);
    std::string exponent = szl::Grammar::toBin(shift);
    if (exponent.length() > 8)
        throw szl::SZLException("Error on float conversion: exponent too big", file, line);
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

std::string szl::Grammar::fromChar(const std::string &in, const std::string &file, const std::string &line)
{
    std::string buf = in.substr(1, in.length() - 2);
    long res = 0;
    if (buf.length() == 1)
        res = buf[0];
    else if (!buf.length())
        throw szl::SZLException("Attempted to assign empty character", file, line);
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
            throw szl::SZLException("Attempted to assign character with unknown value", file, line);
    }
    return szl::Grammar::toBin(res);
}

std::string szl::Grammar::fromDecFloat(const std::string &in, const std::string &file, const std::string &line)
{
    return szl::Grammar::toBinFloat(std::atof(in.c_str()), file, line);
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
        throw szl::SZLException("Syntax error", program[position].file, program[position].line);
    }
    throw szl::SZLException("Scope not concluded at the end of the program", program[position].file, program[position].line);
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
    for (auto &it : grammars)
    {
        it.second->initialize();
    }
    addSubRule("scope closed");
    addSubRule("single variable declaration");
    addSubRule("semicolon");
    addSubRule("lock");
    addSubRule("unlock");
    addSubRule("out");
    addSubRule("function call");
    addSubRule("arrow");
    addSubRule("memcpy");
    addSubRule("free");
    addSubRule("if");
    addSubRule("while");
    addSubRule("for");
    addSubRule("set member field");
    addSubRule("object creation");
    addSubRule("assignment");
    addSubRule("object declaration");
    addSubRule("function declaration");
    addSubRule("return");
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
    grammars.emplace("conversion", new szl::GrammarConversion(this));
    grammars.emplace("get member field", new szl::GrammarGetMemberField(this));
    grammars.emplace("set member field", new szl::GrammarSetMemberField(this));
    grammars.emplace("object declaration", new szl::GrammarObjectDeclaration(this));
    grammars.emplace("object creation", new szl::GrammarObjectCreation(this));
    grammars.emplace("sizeof", new szl::GrammarSizeOf(this));
    grammars.emplace("chained operations", new szl::GrammarChainedOperations(this));
    grammars.emplace("memcpy", new szl::GrammarMemcpy(this));
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
    if (type != "int" && type != "uint" && type != "long" && type != "ulong" && type != "bool" && type != "float" && type != "char" && !szl::objectTypes.count(type))
        return "";

    auto newPos = position + 3;
    auto name = program[position + 1].content;
    std::string subRes = executeSubRules(program, newPos, scope, internalState);
    if (subRes.length() == 0)
        return "";
    position = newPos;

    if (scope.back().exists(name))
        throw szl::SZLException("Redeclaration of variable '" + name + "'", program[position].file, program[position].line);
    if (szl::functions.count(name) > 0)
        throw szl::SZLException("Redeclaration of function '" + name + "'", program[position].file, program[position].line);

    if (internalState.back() != type)
        throw szl::SZLException("Declaring variable of type '" + type + "', but passing value of type '" + internalState.back() + "'", program[position].file, program[position].line);

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

    // OBJECT - already on the stack
    if (szl::objectTypes.count(type) > 0)
    {
        auto object = szl::objectTypes[type];
        scope.back().insertVariable(name, object.getSize(), type);
        return subRes;
    }
    else
    {
        throw szl::SZLException("Unknown type '" + type + "' in variable declaration", program[position].file, program[position].line);
    }

    return subRes;
}

szl::GrammarSingleVariableDeclaration::GrammarSingleVariableDeclaration(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarSingleVariableDeclaration::initialize()
{
    addSubRule("chained operations");
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
        throw szl::SZLException("Assigning to variable '" + name + "', which does not exist in this scope", program[position].file, program[position].line);

    internalState.push_back(scope.back()[name].getType());
    auto size = scope.back()[name].getStackSize();
    if (size == 2)
        return subRes + "LD (#" + std::to_string(scope.back()[name].getPosition()) + "),HL\n";
    if (size == 4)
        return subRes + "LD (#" + std::to_string(scope.back()[name].getPosition()) + "),HL\n" + "LD (#" + std::to_string(scope.back()[name].getPosition() + 2) + "),DE\n";
    else
    {
        if (!szl::objectTypes.count(scope.back()[name].getType()))
            throw szl::SZLException("Unknown type '" + scope.back()[name].getType() + "' in assignment", program[position].file, program[position].line);
        auto object = szl::objectTypes[scope.back()[name].getType()];
        return subRes + "LD DE,#" + std::to_string(scope.back()[name].getPosition()) + "\nLD BC,#" + std::to_string(object.getSize()) + "\nLD H,B\nLD L,C\nADD HL,SP\nLDDR\nLD HL,#" + std::to_string(object.getSize()) + "\nADD HL,SP\nLD SP,HL\n";
    }
}

szl::GrammarAssignment::GrammarAssignment(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarAssignment::initialize()
{
    addSubRule("chained operations");
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
                throw szl::SZLException("Attempting to add malformed values", program[position].file, program[position].line);
            else
                floats = true;
        }
    }
    if (!szl::Grammar::isValidNumber(value2))
    {
        if (value2[0] != '\'' && value2[0] != '"')
        {
            if (!szl::Grammar::isValidFloatNumber(value2))
                throw szl::SZLException("Attempting to add malformed values", program[position].file, program[position].line);
            else
                floats = true;
        }
    }
    position += 3;
    int size = 0; // For string addition only
    if (value1[0] == '\'')
    {
        res1 = szl::Grammar::fromChar(value1, program[position].file, program[position].line);
    }
    else if (value1[0] == '"')
    {
        if (value2[0] != '"')
            throw szl::SZLException("Can not add numerical values to string", program[position].file, program[position].line);
        if (value1[value1.length() - 1] != '"')
            throw szl::SZLException("Malformed string chain", program[position].file, program[position].line);
        size = (value1.length() - 2) * 2;
        for (std::size_t i = 1; i < value1.size() - 1; i++)
        {
            res1 += "LD HL,%" + szl::Grammar::fromChar("'" + value1.substr(i, 1) + "'", program[position].file, program[position].line) + "\nPUSH HL\n";
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
        res2 = szl::Grammar::fromChar(value1, program[position].file, program[position].line);
    }
    else if (value2[0] == '"')
    {
        if (value1[0] != '"')
            throw szl::SZLException("Can not add numerical values to string", program[position].file, program[position].line);
        if (value2[value2.length() - 1] != '"')
            throw szl::SZLException("Malformed string chain", program[position].file, program[position].line);
        size += (value2.length() - 1) * 2;
        int num = 0;
        while (scope.back().exists("[STRINGSAVE" + std::to_string(num) + "]"))
        {
            num++;
        }
        scope.back().insertVariable("[STRINGSAVE" + std::to_string(num) + "]", size, "string");
        for (std::size_t i = 1; i < value2.size() - 1; i++)
        {
            res2 += "LD HL,%" + szl::Grammar::fromChar("'" + value2.substr(i, 1) + "'", program[position].file, program[position].line) + "\nPUSH HL\n";
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
        std::string addRes = szl::Grammar::fromDecFloat(std::to_string(std::atof(res1.c_str()) + std::atof(res2.c_str())), program[position].file, program[position].line);
        internalState.push_back("float");
        return "LD HL,%" +
               addRes.substr(0, 16) + "\nLD DE,%" + addRes.substr(16, 16) + "\n";
    }
    std::string partial = szl::Grammar::binaryAdd(res1, res2);
    if (partial.length() > 32)
        throw szl::SZLException("Added values exceed max allowed size", program[position].file, program[position].line);
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
    addSubRule("chained operations");
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
    auto variable = scope.back()[name];
    if (!szl::objectTypes.count(variable.getType()))
        throw szl::SZLException("Variable with identifier '" + name + "' is of unknown type '" + variable.getType() + "'", program[position].file, program[position].line);
    return "LD BC,#" + std::to_string(szl::objectTypes[variable.getType()].getSize()) + "\nLD HL,%0\nADD HL,SP\nDEC HL\nEX DE,HL\nLD HL,#" + std::to_string(variable.getPosition()) + "\nLDDR\nINC DE\nEX DE,HL\nLD SP,HL\n";
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
                throw szl::SZLException("Literal value exceeds maximum limit", program[position].file, program[position].line);
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
        return "LD HL,%" + szl::Grammar::fromChar(value, program[position].file, program[position].line) + "\n";
    }
    if (szl::Grammar::isValidFloatNumber(value))
    {
        internalState.push_back("float");
        position++;
        auto res = szl::Grammar::fromDecFloat(value, program[position].file, program[position].line);
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
            res += "LD HL,%" + szl::Grammar::fromChar("'" + value.substr(i, 1) + "'", program[position].file, program[position].line) + "\nPUSH HL\n";
        }
        return res + "LD HL,%0\nPUSH HL\nLD HL,SP\n";
    }
    throw szl::SZLException("Can not load given value", program[position].file, program[position].line);
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
        throw szl::SZLException("Addition syntax error", program[position].file, program[position].line);
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("Addition syntax error", program[position].file, program[position].line);
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting addition of differing types", program[position].file, program[position].line);
    internalState.pop_back();

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
        throw szl::SZLException("Bools can not be added", program[position].file, program[position].line);
    }
    throw szl::SZLException("Addition type unrecognized", program[position].file, program[position].line);
}

szl::GrammarAddition::GrammarAddition(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarAddition::initialize()
{
    addSubRule("brackets");
    addSubRule("conversion");
    addSubRule("in");
    addSubRule("function call");
    addSubRule("get member field");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("at");
    addSubRule("questioned at");
    addSubRule("sizeof");
    addSubRule("alloc");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("modulo");
    addSubRule("two literals addition");
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
        throw szl::SZLException("Subtraction syntax error", program[position].file, program[position].line);
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("Subtraction syntax error", program[position].file, program[position].line);
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting subtraction of differing types", program[position].file, program[position].line);
    internalState.pop_back();

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
        throw szl::SZLException("Bools can not be subtracted", program[position].file, program[position].line);
    }
    throw szl::SZLException("Subtraction type unrecognized", program[position].file, program[position].line);
}

szl::GrammarSubtraction::GrammarSubtraction(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarSubtraction::initialize()
{
    addSubRule("brackets");
    addSubRule("conversion");
    addSubRule("in");
    addSubRule("function call");
    addSubRule("get member field");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("at");
    addSubRule("questioned at");
    addSubRule("sizeof");
    addSubRule("alloc");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("modulo");
    addSubRule("two literals addition");
    addSubRule("addition");
    addSubRule("identifier");
    addSubRule("literal");
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
        throw szl::SZLException("Multiplication syntax error", program[position].file, program[position].line);
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("Multiplication syntax error", program[position].file, program[position].line);
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting multiplication of differing types", program[position].file, program[position].line);
    internalState.pop_back();

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
        throw szl::SZLException("Bools can not be multiplied", program[position].file, program[position].line);
    }
    throw szl::SZLException("Multiplication type unrecognized", program[position].file, program[position].line);
}

szl::GrammarMultiplication::GrammarMultiplication(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarMultiplication::initialize()
{
    addSubRule("brackets");
    addSubRule("conversion");
    addSubRule("in");
    addSubRule("function call");
    addSubRule("get member field");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("at");
    addSubRule("questioned at");
    addSubRule("sizeof");
    addSubRule("alloc");
    addSubRule("identifier");
    addSubRule("literal");
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
        throw szl::SZLException("Division syntax error", program[position].file, program[position].line);
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("Division syntax error", program[position].file, program[position].line);
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting division of differing types", program[position].file, program[position].line);
    internalState.pop_back();

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
        throw szl::SZLException("Bools can not be divided", program[position].file, program[position].line);
    }
    throw szl::SZLException("Division type unrecognized", program[position].file, program[position].line);
}

szl::GrammarDivision::GrammarDivision(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarDivision::initialize()
{
    addSubRule("brackets");
    addSubRule("conversion");
    addSubRule("in");
    addSubRule("function call");
    addSubRule("get member field");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("at");
    addSubRule("questioned at");
    addSubRule("sizeof");
    addSubRule("alloc");
    addSubRule("multiplication");
    addSubRule("identifier");
    addSubRule("literal");
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
        throw szl::SZLException("AND syntax error", program[position].file, program[position].line);
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("AND  syntax error", program[position].file, program[position].line);
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting AND with parameters of differing types", program[position].file, program[position].line);
    internalState.pop_back();

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
        throw szl::SZLException("Floats can not be arguments of AND", program[position].file, program[position].line);
    }

    // BOOL
    if (internalState.back() == "bool")
    {
        return resL + "PUSH HL\n" + res + "POP DE\nCALL @stdszllib_and_bool\n";
    }
    throw szl::SZLException("AND operation type unrecognized", program[position].file, program[position].line);
}

szl::GrammarAnd::GrammarAnd(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarAnd::initialize()
{
    addSubRule("brackets");
    addSubRule("conversion");
    addSubRule("in");
    addSubRule("function call");
    addSubRule("get member field");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("at");
    addSubRule("questioned at");
    addSubRule("sizeof");
    addSubRule("alloc");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("modulo");
    addSubRule("two literals addition");
    addSubRule("addition");
    addSubRule("subtraction");
    addSubRule("shift left");
    addSubRule("shift right");
    addSubRule("greater");
    addSubRule("greater or equal");
    addSubRule("less");
    addSubRule("less or equal");
    addSubRule("not equal");
    addSubRule("equal");
    addSubRule("identifier");
    addSubRule("literal");
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
        throw szl::SZLException("OR syntax error", program[position].file, program[position].line);
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("OR  syntax error", program[position].file, program[position].line);
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting OR with parameters of differing types", program[position].file, program[position].line);
    internalState.pop_back();

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
        throw szl::SZLException("Floats can not be arguments of OR", program[position].file, program[position].line);
    }

    // BOOL
    if (internalState.back() == "bool")
    {
        return resL + "PUSH HL\n" + res + "POP DE\nCALL @stdszllib_or_bool\n";
    }
    throw szl::SZLException("OR operation type unrecognized", program[position].file, program[position].line);
}

szl::GrammarOr::GrammarOr(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarOr::initialize()
{
    addSubRule("brackets");
    addSubRule("conversion");
    addSubRule("in");
    addSubRule("function call");
    addSubRule("get member field");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("at");
    addSubRule("questioned at");
    addSubRule("sizeof");
    addSubRule("alloc");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("modulo");
    addSubRule("two literals addition");
    addSubRule("addition");
    addSubRule("subtraction");
    addSubRule("shift left");
    addSubRule("shift right");
    addSubRule("greater");
    addSubRule("greater or equal");
    addSubRule("less");
    addSubRule("less or equal");
    addSubRule("not equal");
    addSubRule("equal");
    addSubRule("and");
    addSubRule("xor");
    addSubRule("identifier");
    addSubRule("literal");
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
        throw szl::SZLException("XOR syntax error", program[position].file, program[position].line);
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("XOR  syntax error", program[position].file, program[position].line);
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting XOR with parameters of differing types", program[position].file, program[position].line);
    internalState.pop_back();

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
        throw szl::SZLException("Floats can not be arguments of XOR", program[position].file, program[position].line);
    }

    // BOOL
    if (internalState.back() == "bool")
    {
        return resL + "PUSH HL\n" + res + "POP DE\nCALL @stdszllib_xor_bool\n";
    }
    throw szl::SZLException("XOR operation type unrecognized", program[position].file, program[position].line);
}

szl::GrammarXor::GrammarXor(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarXor::initialize()
{
    addSubRule("brackets");
    addSubRule("conversion");
    addSubRule("in");
    addSubRule("function call");
    addSubRule("get member field");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("at");
    addSubRule("questioned at");
    addSubRule("sizeof");
    addSubRule("alloc");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("modulo");
    addSubRule("two literals addition");
    addSubRule("addition");
    addSubRule("subtraction");
    addSubRule("shift left");
    addSubRule("shift right");
    addSubRule("greater");
    addSubRule("greater or equal");
    addSubRule("less");
    addSubRule("less or equal");
    addSubRule("not equal");
    addSubRule("equal");
    addSubRule("and");
    addSubRule("identifier");
    addSubRule("literal");
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
        throw szl::SZLException("Modulo syntax error", program[position].file, program[position].line);
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("Modulo syntax error", program[position].file, program[position].line);
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting to get modulo of differing types", program[position].file, program[position].line);
    internalState.pop_back();

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
        throw szl::SZLException("Floats can not be arguments of modulo", program[position].file, program[position].line);
    }

    // BOOL
    if (internalState.back() == "bool")
    {
        throw szl::SZLException("Bools can not be arguments of modulo", program[position].file, program[position].line);
    }
    throw szl::SZLException("Modulo type unrecognized", program[position].file, program[position].line);
}

szl::GrammarModulo::GrammarModulo(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarModulo::initialize()
{
    addSubRule("brackets");
    addSubRule("conversion");
    addSubRule("in");
    addSubRule("function call");
    addSubRule("get member field");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("at");
    addSubRule("questioned at");
    addSubRule("sizeof");
    addSubRule("alloc");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("identifier");
    addSubRule("literal");
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
        throw szl::SZLException("NOT syntax error", program[position].file, program[position].line);
    position = newPos;
    if (internalState.size() < 1)
        throw szl::SZLException("NOT syntax error", program[position].file, program[position].line);

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
        throw szl::SZLException("Floats can not be arguments of NOT", program[position].file, program[position].line);
    }

    // BOOL
    if (internalState.back() == "bool")
    {
        return res + "CALL @stdszllib_negation_16bit\n";
    }
    throw szl::SZLException("NOT operation type unrecognized", program[position].file, program[position].line);
}

szl::GrammarNot::GrammarNot(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarNot::initialize()
{
    addSubRule("brackets");
    addSubRule("conversion");
    addSubRule("in");
    addSubRule("function call");
    addSubRule("get member field");
    addSubRule("identifier");
    addSubRule("literal");
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
        throw szl::SZLException("Negation syntax error", program[position].file, program[position].line);
    position = newPos;
    if (internalState.size() < 1)
        throw szl::SZLException("Negation syntax error", program[position].file, program[position].line);

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
        throw szl::SZLException("Floats can not be arguments of negation", program[position].file, program[position].line);
    }

    // BOOL
    if (internalState.back() == "bool")
    {
        return res + "CALL @stdszllib_negation_16bit\n";
    }
    throw szl::SZLException("Negation operation type unrecognized", program[position].file, program[position].line);
}

szl::GrammarNegation::GrammarNegation(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarNegation::initialize()
{
    addSubRule("brackets");
    addSubRule("conversion");
    addSubRule("in");
    addSubRule("function call");
    addSubRule("get member field");
    addSubRule("not");
    addSubRule("identifier");
    addSubRule("literal");
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
        throw szl::SZLException("Left shift syntax error", program[position].file, program[position].line);
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("Left shift syntax error", program[position].file, program[position].line);
    if (internalState.back() != "uint")
        throw szl::SZLException("Second argument of left shift can only be of type uint", program[position].file, program[position].line);
    internalState.pop_back();

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
        throw szl::SZLException("Floats can not be arguments of left shift", program[position].file, program[position].line);
    }

    // BOOL
    if (internalState.back() == "bool")
    {
        throw szl::SZLException("Bools can not be arguments of left shift", program[position].file, program[position].line);
    }
    throw szl::SZLException("Left shift type unrecognized", program[position].file, program[position].line);
}

szl::GrammarShiftLeft::GrammarShiftLeft(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarShiftLeft::initialize()
{
    addSubRule("brackets");
    addSubRule("conversion");
    addSubRule("in");
    addSubRule("function call");
    addSubRule("get member field");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("at");
    addSubRule("questioned at");
    addSubRule("sizeof");
    addSubRule("alloc");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("modulo");
    addSubRule("two literals addition");
    addSubRule("addition");
    addSubRule("subtraction");
    addSubRule("identifier");
    addSubRule("literal");
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
        throw szl::SZLException("Right shift syntax error", program[position].file, program[position].line);
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("Right shift syntax error", program[position].file, program[position].line);
    if (internalState.back() != "uint")
        throw szl::SZLException("Second argument of right shift can only be of type uint", program[position].file, program[position].line);
    internalState.pop_back();

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
        throw szl::SZLException("Floats can not be arguments of right shift", program[position].file, program[position].line);
    }

    // BOOL
    if (internalState.back() == "bool")
    {
        throw szl::SZLException("Bools can not be arguments of right shift", program[position].file, program[position].line);
    }
    throw szl::SZLException("Right shift type unrecognized", program[position].file, program[position].line);
}

szl::GrammarShiftRight::GrammarShiftRight(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarShiftRight::initialize()
{
    addSubRule("brackets");
    addSubRule("conversion");
    addSubRule("in");
    addSubRule("function call");
    addSubRule("get member field");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("at");
    addSubRule("questioned at");
    addSubRule("sizeof");
    addSubRule("alloc");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("modulo");
    addSubRule("two literals addition");
    addSubRule("addition");
    addSubRule("subtraction");
    addSubRule("shift left");
    addSubRule("identifier");
    addSubRule("literal");
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
        throw szl::SZLException("Not equal comparison syntax error", program[position].file, program[position].line);
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("Not equal comparison syntax error", program[position].file, program[position].line);
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting not equal comparison of differing types", program[position].file, program[position].line);
    internalState.pop_back();

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
    throw szl::SZLException("Not equal comparison type unrecognized", program[position].file, program[position].line);
}

szl::GrammarNotEqual::GrammarNotEqual(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarNotEqual::initialize()
{
    addSubRule("brackets");
    addSubRule("conversion");
    addSubRule("in");
    addSubRule("function call");
    addSubRule("get member field");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("at");
    addSubRule("questioned at");
    addSubRule("sizeof");
    addSubRule("alloc");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("modulo");
    addSubRule("two literals addition");
    addSubRule("addition");
    addSubRule("subtraction");
    addSubRule("shift left");
    addSubRule("shift right");
    addSubRule("greater");
    addSubRule("greater or equal");
    addSubRule("less");
    addSubRule("less or equal");
    addSubRule("identifier");
    addSubRule("literal");
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
        throw szl::SZLException("Equal comparison syntax error", program[position].file, program[position].line);
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("Equal comparison syntax error", program[position].file, program[position].line);
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting equal comparison of differing types", program[position].file, program[position].line);
    internalState.pop_back();

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
    throw szl::SZLException("Equal comparison type unrecognized", program[position].file, program[position].line);
}

szl::GrammarEqual::GrammarEqual(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarEqual::initialize()
{
    addSubRule("brackets");
    addSubRule("conversion");
    addSubRule("in");
    addSubRule("function call");
    addSubRule("get member field");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("at");
    addSubRule("questioned at");
    addSubRule("sizeof");
    addSubRule("alloc");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("modulo");
    addSubRule("two literals addition");
    addSubRule("addition");
    addSubRule("subtraction");
    addSubRule("shift left");
    addSubRule("shift right");
    addSubRule("greater");
    addSubRule("greater or equal");
    addSubRule("less");
    addSubRule("less or equal");
    addSubRule("not equal");
    addSubRule("identifier");
    addSubRule("literal");
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
        throw szl::SZLException("Greater comparison syntax error", program[position].file, program[position].line);
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("Greater comparison syntax error", program[position].file, program[position].line);
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting greater comparison of differing types", program[position].file, program[position].line);
    internalState.pop_back();

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
        throw szl::SZLException("Boolean values can not be arguments of greater comparison", program[position].file, program[position].line);
    }
    throw szl::SZLException("Greater comparison type unrecognized", program[position].file, program[position].line);
}

szl::GrammarGreater::GrammarGreater(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarGreater::initialize()
{
    addSubRule("brackets");
    addSubRule("conversion");
    addSubRule("in");
    addSubRule("function call");
    addSubRule("get member field");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("at");
    addSubRule("questioned at");
    addSubRule("sizeof");
    addSubRule("alloc");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("modulo");
    addSubRule("two literals addition");
    addSubRule("addition");
    addSubRule("subtraction");
    addSubRule("shift left");
    addSubRule("shift right");
    addSubRule("identifier");
    addSubRule("literal");
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
        throw szl::SZLException("Less comparison syntax error", program[position].file, program[position].line);
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("Less comparison syntax error", program[position].file, program[position].line);
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting less comparison of differing types", program[position].file, program[position].line);
    internalState.pop_back();

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
        throw szl::SZLException("Boolean values can not be arguments of less comparison", program[position].file, program[position].line);
    }
    throw szl::SZLException("Less comparison type unrecognized", program[position].file, program[position].line);
}

szl::GrammarLess::GrammarLess(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarLess::initialize()
{
    addSubRule("brackets");
    addSubRule("conversion");
    addSubRule("in");
    addSubRule("function call");
    addSubRule("get member field");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("at");
    addSubRule("questioned at");
    addSubRule("sizeof");
    addSubRule("alloc");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("modulo");
    addSubRule("two literals addition");
    addSubRule("addition");
    addSubRule("subtraction");
    addSubRule("shift left");
    addSubRule("shift right");
    addSubRule("greater");
    addSubRule("greater or equal");
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
        throw szl::SZLException("Greater or equal comparison syntax error", program[position].file, program[position].line);
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("Greater or equal comparison syntax error", program[position].file, program[position].line);
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting greater or equal comparison of differing types", program[position].file, program[position].line);
    internalState.pop_back();

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
        throw szl::SZLException("Boolean values can not be arguments of greater or equal comparison", program[position].file, program[position].line);
    }
    throw szl::SZLException("Greater or equal comparison type unrecognized", program[position].file, program[position].line);
}

szl::GrammarGreaterOrEqual::GrammarGreaterOrEqual(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarGreaterOrEqual::initialize()
{
    addSubRule("brackets");
    addSubRule("conversion");
    addSubRule("in");
    addSubRule("function call");
    addSubRule("get member field");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("at");
    addSubRule("questioned at");
    addSubRule("sizeof");
    addSubRule("alloc");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("modulo");
    addSubRule("two literals addition");
    addSubRule("addition");
    addSubRule("subtraction");
    addSubRule("shift left");
    addSubRule("shift right");
    addSubRule("greater");
    addSubRule("identifier");
    addSubRule("literal");
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
        throw szl::SZLException("Less or equal comparison syntax error", program[position].file, program[position].line);
    position = newPos;
    if (internalState.size() < 2)
        throw szl::SZLException("Less or equal comparison syntax error", program[position].file, program[position].line);
    if (internalState.back() != internalState[internalState.size() - 2])
        throw szl::SZLException("Attempting less or equal comparison of differing types", program[position].file, program[position].line);
    internalState.pop_back();

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
        throw szl::SZLException("Boolean values can not be arguments of less or equal comparison", program[position].file, program[position].line);
    }
    throw szl::SZLException("Less or equal comparison type unrecognized", program[position].file, program[position].line);
}

szl::GrammarLessOrEqual::GrammarLessOrEqual(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarLessOrEqual::initialize()
{
    addSubRule("brackets");
    addSubRule("conversion");
    addSubRule("in");
    addSubRule("function call");
    addSubRule("get member field");
    addSubRule("not");
    addSubRule("negation");
    addSubRule("at");
    addSubRule("questioned at");
    addSubRule("sizeof");
    addSubRule("alloc");
    addSubRule("multiplication");
    addSubRule("division");
    addSubRule("modulo");
    addSubRule("two literals addition");
    addSubRule("addition");
    addSubRule("subtraction");
    addSubRule("shift left");
    addSubRule("shift right");
    addSubRule("greater");
    addSubRule("greater or equal");
    addSubRule("less");
    addSubRule("identifier");
    addSubRule("literal");
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
        throw szl::SZLException("If statement syntax error", program[position].file, program[position].line);
    if (program[newPos].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("If statement syntax error", program[position].file, program[position].line);
    if (program[newPos].content != ")")
        throw szl::SZLException("If statement syntax error", program[position].file, program[position].line);
    position = newPos + 1;
    if (position >= program.size())
        throw szl::SZLException("If statement syntax error: EOF", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("If statement syntax error", program[position].file, program[position].line);
    if (program[position].content != "{")
        throw szl::SZLException("If statement syntax error", program[position].file, program[position].line);
    if (internalState.size() < 1)
        throw szl::SZLException("If statement requires condition", program[position].file, program[position].line);
    if (internalState.back() != "bool")
        throw szl::SZLException("If statement requires boolean condition", program[position].file, program[position].line);
    internalState.pop_back();

    std::string endId = "@szlCompilerIfStatementAbsEndingId" + std::to_string(szl::nextUniqueId++) + "\n";
    res += "LD DE,%0\nOR A\nSBC HL,DE\nJP Z," + scope.back().getNextLabel() + "\n";
    while (true)
    {
        scope.emplace_back(0, &res, &scope.back());
        position++;
        res += compileScope(program, position, scope, internalState);
        scope.back().changeCode(&res);
        if (program.size() <= position)
        {
            scope.pop_back();
            return res + endId;
        }
        if (program[position].category != szl::TokenCategory::Keyword)
        {
            scope.pop_back();
            return res + endId;
        }
        if (program[position].content != "else")
        {
            scope.pop_back();
            return res + endId;
        }
        scope.back().addCustomDeleteCode("JP " + endId);
        scope.pop_back();
        if (program.size() <= position + 1)
            throw szl::SZLException("Unexpected EOF on else", program[position].file, program[position].line);

        bool conditional = true;
        if (program[position = position + 1].category != szl::TokenCategory::Keyword)
            conditional = false;
        if (program[position].content != "if")
            conditional = false;
        if (position + 1 >= program.size())
            throw szl::SZLException("Unexpected EOF on else", program[position].file, program[position].line);
        if (conditional)
        {
            if (program[++position].category != szl::TokenCategory::Bracket)
                throw szl::SZLException("Syntax error on else", program[position].file, program[position].line);
            if (program[position++].content != "(")
                throw szl::SZLException("Syntax error on else", program[position].file, program[position].line);
            res += executeSubRules(program, position, scope, internalState);
            if (!res.length())
                throw szl::SZLException("If else statement syntax error", program[position].file, program[position].line);
            if (program[position].category != szl::TokenCategory::Bracket)
                throw szl::SZLException("If else statement syntax error", program[position].file, program[position].line);
            if (program[position++].content != ")")
                throw szl::SZLException("If else statement syntax error", program[position].file, program[position].line);
            if (position >= program.size())
                throw szl::SZLException("If else statement syntax error: EOF", program[position].file, program[position].line);
            if (program[position].category != szl::TokenCategory::Bracket)
                throw szl::SZLException("If else statement syntax error", program[position].file, program[position].line);
            if (program[position].content != "{")
                throw szl::SZLException("If else statement syntax error", program[position].file, program[position].line);
            if (internalState.size() < 1)
                throw szl::SZLException("If else statement requires condition", program[position].file, program[position].line);
            if (internalState.back() != "bool")
                throw szl::SZLException("If else statement requires boolean condition", program[position].file, program[position].line);
            internalState.pop_back();
            res += "LD DE,%0\nOR A\nSBC HL,DE\nJP Z," + scope.back().getNextLabel() + "\n";
        }
        else
        {
            if (program[position].category != szl::TokenCategory::Bracket)
                throw szl::SZLException("Else statement syntax error", program[position].file, program[position].line);
            if (program[position].content != "{")
                throw szl::SZLException("Else statement syntax error", program[position].file, program[position].line);
        }
    }
}

szl::GrammarIf::GrammarIf(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarIf::initialize()
{
    addSubRule("chained operations");
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
        throw szl::SZLException("While statement syntax error", program[position].file, program[position].line);
    if (program[newPos].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("While statement syntax error", program[position].file, program[position].line);
    if (program[newPos].content != ")")
        throw szl::SZLException("While statement syntax error", program[position].file, program[position].line);
    position = newPos + 1;
    if (position >= program.size())
        throw szl::SZLException("Unexpected EOF on while statement", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("While statement syntax error", program[position].file, program[position].line);
    if (program[position].content != "{")
        throw szl::SZLException("While statement syntax error", program[position].file, program[position].line);
    if (internalState.size() < 1)
        throw szl::SZLException("While statement requires condition", program[position].file, program[position].line);
    if (internalState.back() != "bool")
        throw szl::SZLException("While statement requires boolean condition", program[position].file, program[position].line);
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
    addSubRule("chained operations");
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
        throw szl::SZLException("For statement syntax error", program[position].file, program[position].line);
    // if (program[newPos].category != szl::TokenCategory::Punctuation)
    //     throw szl::SZLException("For statement syntax error");
    // if (program[newPos].content != ";")
    //     throw szl::SZLException("For statement syntax error");
    // position = newPos + 1;
    position = newPos;

    res += executeSubRules(program, position, scope, internalState);
    if (!res.length())
        throw szl::SZLException("For statement syntax error", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Punctuation)
        throw szl::SZLException("For statement syntax error", program[position].file, program[position].line);
    if (program[position].content != ";")
        throw szl::SZLException("For statement syntax error", program[position].file, program[position].line);
    position++;
    if (position >= program.size())
        throw szl::SZLException("Unexpected EOF on for statment", program[position].file, program[position].line);

    auto internalBuffer = internalState;
    std::string endingCode = executeSubRules(program, position, scope, internalState);
    if (!res.length())
        throw szl::SZLException("For statement syntax error", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("For statement syntax error", program[position].file, program[position].line);
    if (program[position].content != ")")
        throw szl::SZLException("For statement syntax error", program[position].file, program[position].line);
    position++;
    internalState = internalBuffer;

    if (position >= program.size())
        throw szl::SZLException("Unexpected EOF on for statement", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("For statement syntax error", program[position].file, program[position].line);
    if (program[position].content != "{")
        throw szl::SZLException("For statement syntax error", program[position].file, program[position].line);
    if (internalState.size() < 1)
        throw szl::SZLException("For statement requires condition", program[position].file, program[position].line);
    if (internalState.back() != "bool")
        throw szl::SZLException("For statement requires boolean condition", program[position].file, program[position].line);
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
    addSubRule("chained operations");
}

szl::Function szl::GrammarFunctionDeclaration::createFunctionTableEntry(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::vector<std::pair<std::string, std::string>> arguments;
    if (program[position].category != szl::TokenCategory::Keyword)
        throw szl::SZLException("Syntax error while declaring new function", program[position].file, program[position].line);
    auto returns = program[position++].content;
    if (position >= program.size())
        throw szl::SZLException("Unexpected EOF while declaring new function", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Identifier)
        throw szl::SZLException("Syntax error while declaring new function", program[position].file, program[position].line);
    auto name = program[position++].content;
    if (szl::functions.count(name) > 0)
        throw szl::SZLException("Redeclaration of function '" + name + "'", program[position].file, program[position].line);
    for (const auto &it : scope)
    {
        if (it.exists(name))
            throw szl::SZLException("Redeclaration of variable '" + name + "'", program[position].file, program[position].line);
    }
    if (position >= program.size())
        throw szl::SZLException("Unexpected EOF while declaring new function", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("Syntax error while declaring new function", program[position].file, program[position].line);
    if (program[position++].content != "(")
        throw szl::SZLException("Syntax error while declaring new function", program[position].file, program[position].line);

    while (program[position].category == szl::TokenCategory::Keyword)
    {
        auto type = program[position].content;
        if (position++ >= program.size())
            throw szl::SZLException("Unexpected EOF while declaring new function", program[position].file, program[position].line);
        if (program[position].category != szl::TokenCategory::Identifier)
            throw szl::SZLException("Syntax error while declaring new function", program[position].file, program[position].line);
        if (type != "int" && type != "uint" && type != "char" && type != "bool" && type != "long" && type != "ulong" && type != "float" && type != "void" && !szl::objectTypes.count(type))
            throw szl::SZLException("Argument type unrecognized while declaring new function", program[position].file, program[position].line);
        arguments.push_back({type, program[position++].content});
        if (position >= program.size())
            throw szl::SZLException("Unexpected EOF while declaring new function", program[position].file, program[position].line);
        if (program[position].category != szl::TokenCategory::Punctuation && program[position].category != szl::TokenCategory::Bracket)
            throw szl::SZLException("Syntax error while declaring new function", program[position].file, program[position].line);
        if (program[position].content != "," && program[position].content != ")")
            throw szl::SZLException("Syntax error while declaring new function", program[position].file, program[position].line);
        position++;
    }
    if (arguments.size() > 0)
        position--;
    if (position >= program.size())
        throw szl::SZLException("Unexpected EOF while declaring new function", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("Syntax error while declaring new function", program[position].file, program[position].line);
    if (program[position++].content != ")")
        throw szl::SZLException("Syntax error while declaring new function", program[position].file, program[position].line);
    if (position >= program.size())
        throw szl::SZLException("Unexpected EOF while declaring new function", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("Syntax error while declaring new function", program[position].file, program[position].line);
    if (program[position++].content != "{")
        throw szl::SZLException("Syntax error while declaring new function", program[position].file, program[position].line);
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
    if (type != "int" && type != "uint" && type != "long" && type != "ulong" && type != "bool" && type != "float" && type != "char" && type != "void" && !szl::objectTypes.count(type))
        return "";
    if (program[position + 2].content != "(")
        return "";
    if (scope.size() > 1)
        throw szl::SZLException("Functions can not be defined inside existing scopes", program[position].file, program[position].line);

    auto function = createFunctionTableEntry(program, position, scope, internalState);
    std::string res = function.getLabel();
    int retSize = 0;

    if (type == "int" || type == "uint" || type == "char" || type == "bool")
        retSize = 2;
    else if (type == "long" || type == "ulong" || type == "float")
        retSize = 4;
    else if (type == "void")
        retSize = 0;
    else if (szl::objectTypes.count(type))
        retSize = szl::objectTypes[type].getSize();
    else
        throw szl::SZLException("Function return type not defined", program[position].file, program[position].line);

    auto arguments = function.getArguments();
    std::list<szl::Scope> newScope;
    newScope.emplace_back(12, &res, &scope.front());
    for (std::size_t i = 0; i < arguments.size(); i++)
    {
        int sz = 0;
        auto t = arguments[i].first;
        if (t == "int" || t == "uint" || t == "char" || t == "bool")
        {
            sz = 2;
        }
        else if (t == "long" || t == "ulong" || t == "float")
        {
            sz = 4;
        }
        else if (szl::objectTypes.count(t))
        {
            sz = szl::objectTypes[t].getSize();
        }
        else
            throw szl::SZLException("Function argument type unknown", program[position].file, program[position].line);
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
        throw szl::SZLException("Unexpected EOF on return", program[position].file, program[position].line);
    return executeSubRules(program, position, scope, internalState) + "\n";
}

szl::GrammarReturn::GrammarReturn(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarReturn::initialize()
{
    addSubRule("chained operations");
}

szl::Function szl::GrammarFunctionCall::getFunctionTableEntry(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    if (program[position].category != szl::TokenCategory::Identifier)
        throw szl::SZLException("Syntax error while calling function", program[position].file, program[position].line);
    auto name = program[position++].content;
    if (position >= program.size())
        throw szl::SZLException("Unexpected EOF while calling function", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("Syntax error while calling function", program[position].file, program[position].line);
    if (program[position++].content != "(")
        throw szl::SZLException("Syntax error while calling function", program[position].file, program[position].line);
    if (szl::functions.count(name) < 1)
        throw szl::SZLException("Function with name '" + name + "' does not exist", program[position].file, program[position].line);
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
            throw szl::SZLException("Expression inside function call does not evaluate to any known value type", program[position].file, program[position].line);
        if (!internalState.size())
            throw szl::SZLException("Expression inside function call does not evaluate to any known value type", program[position].file, program[position].line);
        if (internalState.back() != type)
            throw szl::SZLException("Argument mismatch inside function call: expected '" + type + "', got '" + internalState.back() + "' instead", program[position].file, program[position].line);
        if (type == "int" || type == "uint" || type == "char" || type == "bool")
            subRes += "PUSH HL\n";
        else if (type == "long" || type == "ulong" || type == "float")
            subRes += "PUSH HL\nPUSH DE\n";
        else if (!szl::objectTypes.count(type))
            throw szl::SZLException("Argument type in function call defined as '" + type + "', but this type is not known", program[position].file, program[position].line);
        if (program[position].category != szl::TokenCategory::Punctuation && program[position].category != szl::TokenCategory::Bracket)
            throw szl::SZLException("Syntax error while calling function", program[position].file, program[position].line);
        if (program[position].content != "," && program[position].content != ")")
            throw szl::SZLException("Syntax error while calling function", program[position].file, program[position].line);
        position++;
        res += subRes;
    }
    return res + "CALL " + function.getLabel();
}

szl::GrammarFunctionCall::GrammarFunctionCall(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarFunctionCall::initialize()
{
    addSubRule("chained operations");
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
        throw szl::SZLException("Unexpected EOF while calling 'in'", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("Syntax error while calling 'in'", program[position].file, program[position].line);
    if (program[position++].content != "(")
        throw szl::SZLException("Syntax error while calling 'in'", program[position].file, program[position].line);
    std::string res = executeSubRules(program, position, scope, internalState);
    if (!internalState.size())
        throw szl::SZLException("No valid address specified while calling 'in'", program[position].file, program[position].line);
    if (internalState.back() != "uint")
        throw szl::SZLException("Address is not of type uint while calling 'in'", program[position].file, program[position].line);
    if (program.size() <= position)
        throw szl::SZLException("Unexpected EOF while calling 'in'", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("Syntax error while calling 'in'", program[position].file, program[position].line);
    if (program[position++].content != ")")
        throw szl::SZLException("Syntax error while calling 'in'", program[position].file, program[position].line);
    internalState.back() = "int";
    return res + "LD C,L\nIN L,(C)\nLD H,%0\n";
}

szl::GrammarIn::GrammarIn(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarIn::initialize()
{
    addSubRule("chained operations");
}

std::string szl::GrammarOut::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    if (program[position].category != szl::TokenCategory::Keyword)
        return "";
    if (program[position].content != "out")
        return "";
    if (program.size() <= ++position)
        throw szl::SZLException("Unexpected EOF while calling 'out'", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("Syntax error while calling 'out'", program[position].file, program[position].line);
    if (program[position++].content != "(")
        throw szl::SZLException("Syntax error while calling 'out'", program[position].file, program[position].line);
    std::string res = executeSubRules(program, position, scope, internalState) + "PUSH HL\n";
    if (!internalState.size())
        throw szl::SZLException("No valid address specified while calling 'out'", program[position].file, program[position].line);
    if (internalState.back() != "uint")
        throw szl::SZLException("Address is not of type uint while calling 'out'", program[position].file, program[position].line);
    if (program.size() <= position)
        throw szl::SZLException("Unexpected EOF while calling 'out'", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Punctuation)
        throw szl::SZLException("Syntax error while calling 'out'", program[position].file, program[position].line);
    if (program[position++].content != ",")
        throw szl::SZLException("Syntax error while calling 'out'", program[position].file, program[position].line);
    internalState.pop_back();
    res += executeSubRules(program, position, scope, internalState);
    if (!internalState.size())
        throw szl::SZLException("No valid data specified while calling 'out'", program[position].file, program[position].line);
    if (internalState.back() != "int")
        throw szl::SZLException("Data is not of type int while calling 'out'", program[position].file, program[position].line);
    if (program.size() <= position)
        throw szl::SZLException("Unexpected EOF while calling 'out'", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("Syntax error while calling 'out'", program[position].file, program[position].line);
    if (program[position++].content != ")")
        throw szl::SZLException("Syntax error while calling 'out'", program[position].file, program[position].line);
    internalState.pop_back();
    return res + "POP BC\nOUT (C),L\n";
}

szl::GrammarOut::GrammarOut(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarOut::initialize()
{
    addSubRule("chained operations");
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
        throw szl::SZLException("@ syntax error", program[position].file, program[position].line);
    if (internalState.size() < 1)
        throw szl::SZLException("@ requires memory address as right operand", program[position].file, program[position].line);
    if (internalState.back() != "uint")
        throw szl::SZLException("@ expects uint as right operand", program[position].file, program[position].line);
    internalState.back() = type;

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

    // OBJECT
    if (szl::objectTypes.count(internalState.back()))
    {
        return res + "EX DE,HL\nLD BC,#" + std::to_string(szl::objectTypes[internalState.back()].getSize()) + "\nLD HL,%0\nADD HL,SP\nDEC HL\nEX DE,HL\nLDDR\nINC DE\nEX DE,HL\nLD SP,HL\n";
    }
    throw szl::SZLException("@ type not recognized", program[position].file, program[position].line);
}

szl::GrammarAt::GrammarAt(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarAt::initialize()
{
    addSubRule("chained operations");
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
        throw szl::SZLException("-> syntax error", program[position].file, program[position].line);
    if (internalState.size() < 2)
        throw szl::SZLException("-> requires two operands", program[position].file, program[position].line);
    if (internalState.back() != "uint")
        throw szl::SZLException("-> expects uint as right operand", program[position].file, program[position].line);
    internalState.pop_back();

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

    // OBJECT
    if (szl::objectTypes.count(internalState.back()))
    {
        return resL + res + "\nEX DE,HL\nLD BC,#" + std::to_string(szl::objectTypes[internalState.back()].getSize()) + "\nLD H,B\nLD L,C\nADD HL,SP\nLDDR\nLD HL,#" + std::to_string(szl::objectTypes[internalState.back()].getSize()) + "\nADD HL,SP\nLD SP,HL\n";
    }
    throw szl::SZLException("-> type not recognized", program[position].file, program[position].line);
}

szl::GrammarArrow::GrammarArrow(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarArrow::initialize()
{
    addSubRule("chained operations");
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
        throw szl::SZLException("Unexpected EOF on ?@", program[position].file, program[position].line);
    if (program[position = newPos].category != szl::TokenCategory::Identifier)
        throw szl::SZLException("Syntax error on ?@ functionality", program[position].file, program[position].line);
    if (!scope.back().exists(program[position].content))
        throw szl::SZLException("Requested address of variable '" + program[position].content + "', which does not exist", program[position].file, program[position].line);
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
        throw szl::SZLException("Unexpected EOF on alloc", program[position].file, program[position].line);
    res = executeSubRules(program, position = newPos, scope, internalState);
    if (!res.length())
        throw szl::SZLException("Alloc expects one parameter", program[position].file, program[position].line);
    if (internalState.size() < 1)
        throw szl::SZLException("Alloc expects one parameter", program[position].file, program[position].line);
    if (internalState.back() != "uint")
        throw szl::SZLException("Alloc expects uint parameter", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("Syntax error on alloc", program[position].file, program[position].line);
    if (program[position++].content != ")")
        throw szl::SZLException("Syntax error on alloc", program[position].file, program[position].line);
    return res + "LD DE,@szlCompilerHeapPosition\nCALL @stdszllib_alloc\n";
}

szl::GrammarAlloc::GrammarAlloc(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarAlloc::initialize()
{
    addSubRule("chained operations");
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
        throw szl::SZLException("Unexpected EOF on free", program[position].file, program[position].line);
    resL = executeSubRules(program, position = newPos, scope, internalState);
    if (!resL.length())
        throw szl::SZLException("Free expects two parameters", program[position].file, program[position].line);
    if (internalState.size() < 1)
        throw szl::SZLException("Alloc expects two parameters", program[position].file, program[position].line);
    if (internalState.back() != "uint")
        throw szl::SZLException("Alloc expects two uint parameters", program[position].file, program[position].line);
    if (position >= program.size())
        throw szl::SZLException("Unexpected EOF on free", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Punctuation)
        throw szl::SZLException("Syntax error on free", program[position].file, program[position].line);
    if (program[position].content != ",")
        throw szl::SZLException("Syntax error on free", program[position].file, program[position].line);
    if (++position >= program.size())
        throw szl::SZLException("Unexpected EOF on free", program[position].file, program[position].line);
    res = executeSubRules(program, position, scope, internalState);
    if (!res.length())
        throw szl::SZLException("Free expects two parameters", program[position].file, program[position].line);
    if (internalState.size() < 2)
        throw szl::SZLException("Alloc expects two parameters", program[position].file, program[position].line);
    if (internalState.back() != "uint")
        throw szl::SZLException("Alloc expects two uint parameters", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("Syntax error on alloc", program[position].file, program[position].line);
    if (program[position++].content != ")")
        throw szl::SZLException("Syntax error on alloc", program[position].file, program[position].line);
    internalState.pop_back();
    internalState.pop_back();
    return resL + "PUSH HL\n" + res + "POP DE\nEX DE,HL\nCALL @stdszllib_free\n";
}

szl::GrammarFree::GrammarFree(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarFree::initialize()
{
    addSubRule("chained operations");
}

std::string szl::GrammarConversion::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    auto newPos = position;
    std::string res;
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return "";
    auto type = program[newPos++].content;
    if (type != "int" && type != "uint" && type != "char" && type != "long" && type != "ulong" && type != "float")
        return "";
    if (program.size() <= newPos)
        return "";
    if (program[newPos].category != szl::TokenCategory::Bracket)
        return "";
    if (program[newPos++].content != "(")
        return "";
    if (program.size() <= newPos)
        throw szl::SZLException("Unexpected EOF when converting to " + type, program[position].file, program[position].line);
    res = executeSubRules(program, position = newPos, scope, internalState);
    if (!res.length())
        throw szl::SZLException("Conversion to " + type + " expects one parameter", program[position].file, program[position].line);
    if (internalState.size() < 1)
        throw szl::SZLException("Conversion to " + type + " expects one parameter", program[position].file, program[position].line);
    if (internalState.back() != "int" && internalState.back() != "uint" && internalState.back() != "char" && internalState.back() != "long" && internalState.back() != "ulong" && internalState.back() != "float")
        throw szl::SZLException("Conversion to " + type + " expects one parameter of type int, uint, char, long, ulong or float", program[position].file, program[position].line);
    if (program.size() <= position)
        throw szl::SZLException("Unexpected EOF when converting " + internalState.back() + " to " + type, program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("Syntax error when converting " + internalState.back() + " to " + type, program[position].file, program[position].line);
    if (program[position++].content != ")")
        throw szl::SZLException("Syntax error when converting " + internalState.back() + " to " + type, program[position].file, program[position].line);
    internalState.back() = type;
    if (internalState.back() == type)
        return res;
    if (internalState.back() == "float")
    {
        if (type == "long")
            return res + "CALL @stdszllib_conversion_float_to_long\n";
        if (type == "ulong")
            return res + "CALL @stdszllib_conversion_float_to_ulong\n";
        if (type == "int")
            return res + "CALL @stdszllib_conversion_float_to_int\n";
        if (type == "uint" || type == "char")
            return res + "CALL @stdszllib_conversion_float_to_uint\n";
    }
    else if (internalState.back() == "long")
    {
        if (type == "float")
            return res + "CALL @stdszllib_conversion_long_to_float\n";
        if (type == "ulong")
            return res;
        if (type == "int" || type == "uint" || type == "char")
            return res + "EX DE,HL\n";
    }
    else if (internalState.back() == "ulong")
    {
        if (type == "float")
            return res + "CALL @stdszllib_conversion_ulong_to_float\n";
        if (type == "long")
            return res;
        if (type == "int" || type == "uint" || type == "char")
            return res + "EX DE,HL\n";
    }
    else if (internalState.back() == "int")
    {
        if (type == "float")
            return res + "CALL @stdszllib_conversion_int_to_float\n";
        if (type == "long" || type == "ulong")
            return res + "EX DE,HL\nLD HL,%0\n";
        if (type == "uint" || type == "char")
            return res;
    }
    else if (internalState.back() == "uint" || internalState.back() == "char")
    {
        if (type == "float")
            return res + "CALL @stdszllib_conversion_uint_to_float\n";
        if (type == "long" || type == "ulong")
            return res + "EX DE,HL\nLD HL,%0\n";
        if (type == "int")
            return res;
    }
    throw szl::SZLException("No function " + type + "(" + internalState.back() + ") found", program[position].file, program[position].line);
}

szl::GrammarConversion::GrammarConversion(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarConversion::initialize()
{
    addSubRule("chained operations");
}

std::string szl::GrammarGetMemberField::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    if (program[position].category != szl::TokenCategory::Identifier)
        return "";
    auto name = program[position].content;
    if (!scope.back().exists(name))
        return "";
    auto variable = scope.back()[name];
    if (!szl::objectTypes.count(variable.getType()))
        return "";
    auto object = szl::objectTypes[variable.getType()];
    if (++position >= program.size())
        throw szl::SZLException("Unexpected EOF while accessing object", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Punctuation)
        throw szl::SZLException("Syntax error while accessing object", program[position].file, program[position].line);
    if (program[position++].content != ".")
        throw szl::SZLException("Syntax error while accessing object", program[position].file, program[position].line);
    if (position >= program.size())
        throw szl::SZLException("Unexpected EOF while accessing object", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Identifier)
        throw szl::SZLException("Syntax error while accessing object", program[position].file, program[position].line);
    auto memberName = program[position].content;
    if (!object.getContents().count(memberName))
        throw szl::SZLException("Object '" + object.getName() + "' does not have member named '" + memberName + "'", program[position].file, program[position].line);
    auto member = object.getContents()[memberName];
    internalState.push_back(member);
    if (!szl::objectTypes.count(member))
    {
        if (member == "int" || member == "uint" || member == "char" || member == "bool")
            return "LD HL,#" + std::to_string(object.getVariablePosition(variable.getPosition(), memberName)) + "\nLD D,(HL)\nDEC HL\nLD E,(HL)\nEX DE,HL\n";
        if (member == "long" || member == "ulong" || member == "float")
            return "LD HL,#" + std::to_string(object.getVariablePosition(variable.getPosition(), memberName)) + "\nLD B,(HL)\nDEC HL\nLD C,(HL)\nDEC HL\nLD D,(HL)\nDEC HL\nLD E,(HL)\nLD H,B\nLD L,C\n";
        throw szl::SZLException("Member named '" + memberName + "' type '" + member + "' not recognized", program[position].file, program[position].line);
    }
    return "LD HL,%0\nADD HL,SP\nDEC HL\n EX DE,HL\nLD HL,#" + std::to_string(object.getVariablePosition(variable.getPosition(), memberName)) + "\nLD BC,#" + std::to_string(szl::objectTypes[member].getSize()) + "\nLDDR\nINC DE\nEX DE,HL\nLD SP,HL\n";
}

szl::GrammarGetMemberField::GrammarGetMemberField(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarGetMemberField::initialize()
{
}

std::string szl::GrammarSetMemberField::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    if (program[position].category != szl::TokenCategory::Identifier)
        return "";
    auto name = program[position].content;
    if (!scope.back().exists(name))
        return "";
    auto variable = scope.back()[name];
    if (!szl::objectTypes.count(variable.getType()))
        return "";
    if (position + 4 >= program.size())
        return "";
    if (program[position + 3].category != szl::TokenCategory::Keyword)
        return "";
    if (program[position + 3].content != "=")
        return "";
    auto object = szl::objectTypes[variable.getType()];
    if (++position >= program.size())
        throw szl::SZLException("Unexpected EOF while accessing object", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Punctuation)
        throw szl::SZLException("Syntax error while accessing object", program[position].file, program[position].line);
    if (program[position++].content != ".")
        throw szl::SZLException("Syntax error while accessing object", program[position].file, program[position].line);
    if (position >= program.size())
        throw szl::SZLException("Unexpected EOF while accessing object", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Identifier)
        throw szl::SZLException("Syntax error while accessing object", program[position].file, program[position].line);
    auto memberName = program[position].content;
    if (!object.getContents().count(memberName))
        throw szl::SZLException("Object '" + object.getName() + "' does not have member named '" + memberName + "'", program[position].file, program[position].line);
    auto member = object.getContents()[memberName];
    auto pos = std::to_string(object.getVariablePosition(variable.getPosition(), memberName));
    auto res = executeSubRules(program, position = position + 2, scope, internalState);
    if (!res.length())
        throw szl::SZLException("No valid value to assign to member '" + memberName + "'", program[position].file, program[position].line);
    if (!internalState.size())
        throw szl::SZLException("No valid value to assign to member '" + memberName + "'", program[position].file, program[position].line);
    if (internalState.back() != member)
        throw szl::SZLException("Attempting to write value of type '" + internalState.back() + "' to member '" + memberName + "', which is of type '" + member + "'", program[position].file, program[position].line);
    internalState.pop_back();
    if (!szl::objectTypes.count(member))
    {
        if (member == "int" || member == "uint" || member == "char" || member == "bool")
            return res + "EX DE,HL\nLD HL,#" + pos + "\nLD (HL),D\nDEC HL\nLD (HL),E\n";
        if (member == "long" || member == "ulong" || member == "float")
            return res + "LD B,H\nLD C,L\nLD HL,#" + pos + "\nLD (HL),B\nDEC HL\nLD (HL),C\nDEC HL\nLD (HL),D\nDEC HL\nLD (HL),E\n";
        throw szl::SZLException("Member named '" + memberName + "' type '" + member + "' not recognized", program[position].file, program[position].line);
    }
    return res + "LD DE,#" + pos + "\nLD HL,#" + std::to_string(szl::objectTypes[member].getSize()) + "\nLD B,H\nLD C,L\nADD HL,SP\nLDDR\nLD HL,#" + std::to_string(szl::objectTypes[member].getSize()) + "\nADD HL,SP\nLD SP,HL\n";
}

szl::GrammarSetMemberField::GrammarSetMemberField(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarSetMemberField::initialize()
{
    addSubRule("chained operations");
}

std::string szl::GrammarObjectDeclaration::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    if (program[position].category != szl::TokenCategory::Keyword)
        return "";
    if (program[position].content != "object")
        return "";
    if (scope.size() > 1)
        throw szl::SZLException("Objects can only be declared in the global scope", program[position].file, program[position].line);
    if (++position >= program.size())
        throw szl::SZLException("Unexpected EOF while declaring object", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Identifier)
        throw szl::SZLException("Syntax error while declaring object", program[position].file, program[position].line);
    auto name = program[position].content;
    if (szl::objectTypes.count(name))
        throw szl::SZLException("Object named '" + name + "' already exists", program[position].file, program[position].line);
    if (++position >= program.size())
        throw szl::SZLException("Unexpected EOF while declaring object", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("Syntax error while declaring object", program[position].file, program[position].line);
    if (program[position++].content != "{")
        throw szl::SZLException("Syntax error while declaring object", program[position].file, program[position].line);
    if (position >= program.size())
        throw szl::SZLException("Unexpected EOF while declaring object", program[position].file, program[position].line);
    szl::objectTypes.emplace(name, szl::Object());
    szl::objectTypes[name].setName(name);
    for (int offset = 0; position + 3 < program.size();)
    {
        if (program[position].category == szl::TokenCategory::Bracket)
        {
            if (program[position].content == "}")
                break;
        }
        if (program[position].category != szl::TokenCategory::Keyword && program[position].category != szl::TokenCategory::Identifier)
            throw szl::SZLException("Syntax error while declaring member field of object", program[position].file, program[position].line);
        auto type = program[position++].content;
        if (program[position].category != szl::TokenCategory::Identifier)
            throw szl::SZLException("Syntax error while declaring member field of object", program[position].file, program[position].line);
        auto memberName = program[position++].content;
        if (program[position].category != szl::TokenCategory::Punctuation)
            throw szl::SZLException("Syntax error while declaring member field of object", program[position].file, program[position].line);
        if (program[position++].content != ";")
            throw szl::SZLException("Syntax error while declaring member field of object", program[position].file, program[position].line);
        if (!szl::objectTypes.count(type))
        {
            if (type != "int" && type != "uint" && type != "char" && type != "bool" && type != "long" && type != "ulong" && type != "float")
                throw szl::SZLException("Member field type '" + type + "' not recognized", program[position].file, program[position].line);
            int size = 0;
            if (type == "int" || type == "uint" || type == "char" || type == "bool")
                size = 2;
            else if (type == "long" || type == "ulong" || type == "float")
                size = 4;
            szl::objectTypes[name].getContents().emplace(memberName, type);
            szl::objectTypes[name].getVariables().push_back({memberName, szl::Variable(offset, size, type)});
            offset += size;
        }
    }
    return "\n";
}

szl::GrammarObjectDeclaration::GrammarObjectDeclaration(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarObjectDeclaration::initialize()
{
}

std::string szl::GrammarObjectCreation::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    if (program[position].category != szl::TokenCategory::Identifier)
        return "";
    auto type = program[position].content;
    if (!szl::objectTypes.count(type))
        return "";
    auto newPos = position + 1;
    if (newPos >= program.size())
        throw szl::SZLException("Unexpected EOF while creating object of type '" + type + "'", program[position].file, program[position].line);
    if (program[newPos].category != szl::TokenCategory::Identifier)
        throw szl::SZLException("Syntax error while creating object", program[position].file, program[position].line);
    auto name = program[newPos++].content;
    if (scope.back().exists(name))
        throw szl::SZLException("Redeclaration of variable '" + name + "'", program[position].file, program[position].line);
    if (functions.count(name))
        throw szl::SZLException("Redeclaration of function '" + name + "'", program[position].file, program[position].line);
    if (position = newPos + 1 >= program.size())
        throw szl::SZLException("Unexpected EOF while creating object", program[position - 1].file, program[position - 1].line);
    if (program[position].category != szl::TokenCategory::Punctuation)
        throw szl::SZLException("Syntax error while creating object", program[position].file, program[position].line);
    if (program[position++].content != ";")
        throw szl::SZLException("Syntax error while creating object", program[position].file, program[position].line);
    auto object = szl::objectTypes[type];
    scope.back().insertVariable(name, object.getSize(), type);
    return "LD DE,#" + std::to_string(object.getSize()) + "LD HL,%0\nADD HL,SP\nOR A\nSBC HL,DE\nLD SP,HL\n";
}

szl::GrammarObjectCreation::GrammarObjectCreation(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarObjectCreation::initialize()
{
}

std::string szl::GrammarSizeOf::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    if (program[position].category != szl::TokenCategory::Keyword)
        return "";
    if (program[position].content != "sizeof")
        return "";
    if (program.size() <= ++position)
        throw szl::SZLException("Unexpected EOF while calling 'sizeof'", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("Syntax error while calling 'sizeof'", program[position].file, program[position].line);
    if (program[position++].content != "(")
        throw szl::SZLException("Syntax error while calling 'sizeof'", program[position].file, program[position].line);
    if (program.size() <= position)
        throw szl::SZLException("Unexpected EOF while calling 'sizeof'", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Identifier && program[position].category != szl::TokenCategory::Keyword)
        throw szl::SZLException("Syntax error while calling 'sizeof'", program[position].file, program[position].line);
    auto type = program[position++].content;
    position++;
    internalState.push_back("uint");
    if (!szl::objectTypes.count(type))
    {
        if (type == "int" || type == "uint" || type == "char" || type == "bool")
            return "LD HL,#2\n";
        if (type == "long" || type == "ulong" || type == "float")
            return "LD HL,#4\n";
        throw szl::SZLException("Type '" + type + "' not recognized", program[position].file, program[position].line);
    }
    auto size = fromDec(std::to_string(szl::objectTypes[type].getSize()));
    if (size.length() > 16)
        throw szl::SZLException("Size of object '" + type + "' is too large", program[position].file, program[position].line);
    return "LD HL,%" + size + "\n";
}

szl::GrammarSizeOf::GrammarSizeOf(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarSizeOf::initialize()
{
}

std::string szl::GrammarChainedOperations::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{
    std::string res, in;
    do
    {
        in = "";
        in = executeSubRules(program, position, scope, internalState);
        res += in;
    } while (in.length());
    return res;
}

szl::GrammarChainedOperations::GrammarChainedOperations(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarChainedOperations::initialize()
{
    addSubRule("brackets");
    addSubRule("conversion");
    addSubRule("in");
    addSubRule("function call");
    addSubRule("get member field");
    addSubRule("at");
    addSubRule("questioned at");
    addSubRule("sizeof");
    addSubRule("alloc");
    addSubRule("or");
    addSubRule("xor");
    addSubRule("and");
    addSubRule("equal");
    addSubRule("not equal");
    addSubRule("less or equal");
    addSubRule("less");
    addSubRule("greater or equal");
    addSubRule("greater");
    addSubRule("shift right");
    addSubRule("shift left");
    addSubRule("subtraction");
    addSubRule("two literals addition");
    addSubRule("addition");
    addSubRule("modulo");
    addSubRule("division");
    addSubRule("multiplication");
    addSubRule("negation");
    addSubRule("not");
    addSubRule("identifier");
    addSubRule("literal");
}

std::string szl::GrammarMemcpy::execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const
{

    auto newPos = position;
    std::string res, resL, resR;
    if (program[newPos].category != szl::TokenCategory::Keyword)
        return "";
    if (program[newPos].content != "memcpy")
        return "";
    if (++newPos >= program.size())
        return "";
    if (program[newPos].category != szl::TokenCategory::Bracket)
        return "";
    if (program[newPos].content != "(")
        return "";
    if (++newPos >= program.size())
        throw szl::SZLException("Unexpected EOF on memcpy", program[position].file, program[position].line);
    resL = executeSubRules(program, position = newPos, scope, internalState);
    if (!resL.length())
        throw szl::SZLException("Memcpy expects three parameters", program[position].file, program[position].line);
    if (internalState.size() < 1)
        throw szl::SZLException("Memcpy expects three parameters", program[position].file, program[position].line);
    if (internalState.back() != "uint")
        throw szl::SZLException("Memcpy expects three uint parameters", program[position].file, program[position].line);

    if (position >= program.size())
        throw szl::SZLException("Unexpected EOF on memcpy", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Punctuation)
        throw szl::SZLException("Syntax error on memcpy", program[position].file, program[position].line);
    if (program[position].content != ",")
        throw szl::SZLException("Syntax error on memcpy", program[position].file, program[position].line);
    if (++position >= program.size())
        throw szl::SZLException("Unexpected EOF on memcpy", program[position].file, program[position].line);
    res = executeSubRules(program, position, scope, internalState);
    if (!res.length())
        throw szl::SZLException("Memcpy expects three parameters", program[position].file, program[position].line);
    if (internalState.size() < 2)
        throw szl::SZLException("Memcpy expects three parameters", program[position].file, program[position].line);
    if (internalState.back() != "uint")
        throw szl::SZLException("Memcpy expects three uint parameters", program[position].file, program[position].line);

    if (position >= program.size())
        throw szl::SZLException("Unexpected EOF on memcpy", program[position].file, program[position].line);
    if (program[position].category != szl::TokenCategory::Punctuation)
        throw szl::SZLException("Syntax error on memcpy", program[position].file, program[position].line);
    if (program[position].content != ",")
        throw szl::SZLException("Syntax error on memcpy", program[position].file, program[position].line);
    if (++position >= program.size())
        throw szl::SZLException("Unexpected EOF on memcpy", program[position].file, program[position].line);
    resR = executeSubRules(program, position, scope, internalState);
    if (!res.length())
        throw szl::SZLException("Memcpy expects three parameters", program[position].file, program[position].line);
    if (internalState.size() < 3)
        throw szl::SZLException("Memcpy expects three parameters", program[position].file, program[position].line);
    if (internalState.back() != "uint")
        throw szl::SZLException("Memcpy expects three uint parameters", program[position].file, program[position].line);

    if (program[position].category != szl::TokenCategory::Bracket)
        throw szl::SZLException("Syntax error on memcpy", program[position].file, program[position].line);
    if (program[position++].content != ")")
        throw szl::SZLException("Syntax error on memcpy", program[position].file, program[position].line);
    internalState.pop_back();
    internalState.pop_back();
    internalState.pop_back();
    return resL + "PUSH HL\n" + res + "PUSH HL\n" + resR + "LD B,H\nLD C,L\nPOP DE\nPOP HL\nLDIR\n";
}

szl::GrammarMemcpy::GrammarMemcpy(Grammar *root) : szl::Grammar(root) {}

void szl::GrammarMemcpy::initialize()
{
    addSubRule("chained operations");
}