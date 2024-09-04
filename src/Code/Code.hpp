#pragma once
#include <iostream>
#include <vector>

namespace szl
{
    struct CodeLine
    {
        std::string contents;
        std::size_t position;
        std::string file;
    };

    class Code
    {
        std::vector<CodeLine> code;
        std::size_t currentPosition;

    public:
        Code(std::vector<CodeLine> newCode = std::vector<CodeLine>()) : code(newCode) {}

        void insert(CodeLine line, std::size_t position);

        /**
         * @brief Inserts a code line after the current line and set currentPosition to it.
         *
         * @param line line to be inserted
         */
        void insert(CodeLine line);

        /**
         * @brief Inserts a code snippet after the current line and set currentPosition to it. File and position are copied from last currentElement.
         *
         * @param codeText code snippet to be inserted
         */
        void insert(std::string codeText);

        CodeLine &operator[](std::size_t position);

        std::size_t getCurrentPosition() const { return currentPosition; }
        void setCurrentPosition(const std::size_t &currentPosition_) { currentPosition = currentPosition_; }
        void operator++() { currentPosition++; }
    };
}