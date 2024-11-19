#pragma once
#include <iostream>
#include <vector>
#include "Rules/Rule.hpp"
#include "Scope/Scope.hpp"
#include "SZLException/SZLException.hpp"
#include <cmath>
#include <vector>

namespace szl
{

    class Grammar
    {
        static std::string toBin(uint32_t in);

        static std::string toBinFloat(double in);

    protected:
        static std::string fromOct(const std::string &in);

        static std::string fromHex(const std::string &in);

        static std::string fromBin(const std::string &in);

        static std::string fromDec(const std::string &in);

        static std::string fromChar(const std::string &in);

        static std::string fromDecFloat(const std::string &in);

        static bool isValidNumber(const std::string &in);

        static bool isValidFloatNumber(const std::string &in);

        std::vector<Grammar *> subRules;

        std::string executeSubRules(std::vector<szl::Token> &program, std::size_t &position, szl::Scope &scope) const;

        static std::string binaryAdd(std::string in1, std::string in2);

    public:
        /**
         * @brief Attempts to generate assembly code based on given grammar rule, if it is possible to match it.
         *
         * @param program internal tokenized representation of the program
         * @param position starting position reference, it will be moved to the first non-matched token
         * @param scope current scope
         * @return assembly code if matched, emptry string otherwise
         */
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, szl::Scope &scope) const;

        void addSubRule(Grammar *gRule);

        Grammar(bool addSubRules = false);

        virtual ~Grammar();
    };

    class GrammarSingleVariableDeclaration : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, szl::Scope &scope) const override;

        GrammarSingleVariableDeclaration(bool addSubRules = false);
    };

    class GrammarTwoLiteralsAddition : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, szl::Scope &scope) const override;
    };

    class GrammarBracketsRecursive : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, szl::Scope &scope) const override;

        GrammarBracketsRecursive(bool addSubRules = false);
    };

    class GrammarSemicolon : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, szl::Scope &scope) const override;
    };

    class GrammarIdentifier : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, szl::Scope &scope) const override;
    };

    class GrammarLiteral : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, szl::Scope &scope) const override;
    };

} // namespace szl