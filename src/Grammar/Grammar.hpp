#pragma once
#include <iostream>
#include <vector>
#include <list>
#include <utility>
#include "Rules/Rule.hpp"
#include "Scope/Scope.hpp"
#include "Tables/Tables.hpp"
#include "Function/Function.hpp"
#include "SZLException/SZLException.hpp"
#include "NextUniqueId/NextUniqueId.hpp"
#include <cmath>
#include <unordered_map>

namespace szl
{

    class Grammar
    {
        static std::string toBin(uint32_t in);

        std::unordered_map<std::string, Grammar *> grammars;

    protected:
        static std::string fromOct(const std::string &in);

        static std::string fromHex(const std::string &in);

        static std::string fromBin(const std::string &in);

        static std::string fromDec(const std::string &in);

        static std::string fromChar(const std::string &in, const std::string &file, const std::string &line);

        static bool isValidNumber(const std::string &in);

        std::vector<Grammar *> subRules;

        Grammar *root;

        std::string executeSubRules(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const;

        static std::string binaryAdd(std::string in1, std::string in2);

        Grammar *getGrammar(const std::string &name);

        std::string compileScope(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const;

        std::string returnWithClear(int amount, std::vector<std::string> &internalState) const;

    public:
        /**
         * @brief Attempts to generate assembly code based on given grammar rule, if it is possible to match it.
         *
         * @param program internal tokenized representation of the program
         * @param position starting position reference, it will be moved to the first non-matched token
         * @param scope current scope
         * @return assembly code if matched, emptry string otherwise
         */
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const;

        void addSubRule(const std::string id);

        Grammar(Grammar *root = nullptr);

        ~Grammar();

        virtual void initialize();
    };

    class GrammarSingleVariableDeclaration : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarSingleVariableDeclaration(Grammar *root);

        virtual void initialize();
    };

    class GrammarTwoLiteralsAddition : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarTwoLiteralsAddition(Grammar *root);

        virtual void initialize();
    };

    class GrammarBrackets : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarBrackets(Grammar *root);

        virtual void initialize();
    };

    class GrammarSemicolon : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarSemicolon(Grammar *root);

        virtual void initialize();
    };

    class GrammarIdentifier : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarIdentifier(Grammar *root);

        virtual void initialize();
    };

    class GrammarLiteral : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarLiteral(Grammar *root);

        virtual void initialize();
    };

    class GrammarAddition : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarAddition(Grammar *root);

        virtual void initialize();
    };

    class GrammarSubtraction : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarSubtraction(Grammar *root);

        virtual void initialize();
    };

    class GrammarMultiplication : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarMultiplication(Grammar *root);

        virtual void initialize();
    };

    class GrammarDivision : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarDivision(Grammar *root);

        virtual void initialize();
    };

    class GrammarAnd : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarAnd(Grammar *root);

        virtual void initialize();
    };

    class GrammarOr : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarOr(Grammar *root);

        virtual void initialize();
    };

    class GrammarXor : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarXor(Grammar *root);

        virtual void initialize();
    };

    class GrammarModulo : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarModulo(Grammar *root);

        virtual void initialize();
    };

    class GrammarNot : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarNot(Grammar *root);

        virtual void initialize();
    };

    class GrammarNegation : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarNegation(Grammar *root);

        virtual void initialize();
    };

    class GrammarShiftLeft : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarShiftLeft(Grammar *root);

        virtual void initialize();
    };

    class GrammarShiftRight : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarShiftRight(Grammar *root);

        virtual void initialize();
    };

    class GrammarNotEqual : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarNotEqual(Grammar *root);

        virtual void initialize();
    };

    class GrammarEqual : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarEqual(Grammar *root);

        virtual void initialize();
    };

    class GrammarGreater : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarGreater(Grammar *root);

        virtual void initialize();
    };

    class GrammarLess : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarLess(Grammar *root);

        virtual void initialize();
    };

    class GrammarLessOrEqual : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarLessOrEqual(Grammar *root);

        virtual void initialize();
    };

    class GrammarGreaterOrEqual : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarGreaterOrEqual(Grammar *root);

        virtual void initialize();
    };

    class GrammarIf : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarIf(Grammar *root);

        virtual void initialize();
    };

    class GrammarScopeClosed : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarScopeClosed(Grammar *root);

        virtual void initialize();
    };

    class GrammarWhile : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarWhile(Grammar *root);

        virtual void initialize();
    };

    class GrammarFor : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarFor(Grammar *root);

        virtual void initialize();
    };

    class GrammarAssignment : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarAssignment(Grammar *root);

        virtual void initialize();
    };

    class GrammarFunctionDeclaration : public Grammar
    {
        szl::Function createFunctionTableEntry(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const;

    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarFunctionDeclaration(Grammar *root);

        virtual void initialize();
    };

    class GrammarReturn : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarReturn(Grammar *root);

        virtual void initialize();
    };

    class GrammarFunctionCall : public Grammar
    {
        szl::Function getFunctionTableEntry(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const;

    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarFunctionCall(Grammar *root);

        virtual void initialize();
    };

    class GrammarLock : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarLock(Grammar *root);

        virtual void initialize();
    };

    class GrammarUnlock : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarUnlock(Grammar *root);

        virtual void initialize();
    };

    class GrammarIn : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarIn(Grammar *root);

        virtual void initialize();
    };

    class GrammarOut : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarOut(Grammar *root);

        virtual void initialize();
    };

    class GrammarAt : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarAt(Grammar *root);

        virtual void initialize();
    };

    class GrammarArrow : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarArrow(Grammar *root);

        virtual void initialize();
    };

    class GrammarQuestionedAt : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarQuestionedAt(Grammar *root);

        virtual void initialize();
    };

    class GrammarAlloc : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarAlloc(Grammar *root);

        virtual void initialize();
    };

    class GrammarFree : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarFree(Grammar *root);

        virtual void initialize();
    };

    class GrammarConversion : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarConversion(Grammar *root);

        virtual void initialize();
    };

    class GrammarGetMemberField : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarGetMemberField(Grammar *root);

        virtual void initialize();
    };

    class GrammarSetMemberField : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarSetMemberField(Grammar *root);

        virtual void initialize();
    };

    class GrammarObjectDeclaration : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarObjectDeclaration(Grammar *root);

        virtual void initialize();
    };

    class GrammarObjectCreation : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarObjectCreation(Grammar *root);

        virtual void initialize();
    };

    class GrammarSizeOf : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarSizeOf(Grammar *root);

        virtual void initialize();
    };

    class GrammarChainedOperations : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarChainedOperations(Grammar *root);

        virtual void initialize();
    };

    class GrammarMemcpy : public Grammar
    {
    public:
        virtual std::string execute(std::vector<szl::Token> &program, std::size_t &position, std::list<szl::Scope> &scope, std::vector<std::string> &internalState) const override;

        GrammarMemcpy(Grammar *root);

        virtual void initialize();
    };

} // namespace szl