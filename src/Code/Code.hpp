#pragma once
#include <iostream>
#include <vector>
#include <filesystem>

namespace szl
{
    class CodeCharacter
    {
    public:
        char contents;
        std::string file;
        std::string line;
        bool operator==(const char &ch) const;
        operator char() const;
    };

    class Code
    {
        std::vector<CodeCharacter> code;

        std::vector<std::filesystem::path> includedFiles;

    public:
        Code(std::vector<CodeCharacter> newCode = std::vector<CodeCharacter>()) : code(newCode) {}

        void insert(std::string codeText, std::size_t position, std::string file, std::size_t line);

        void insert(const szl::Code &codeText, std::size_t position);

        void insert(const szl::CodeCharacter &codeChar, std::size_t position);

        const CodeCharacter &operator[](std::size_t position) const;

        std::size_t size() const;

        std::size_t length() const;

        std::string substr(const std::size_t position, const std::size_t length) const;

        bool isFileIncluded(const std::string &files) const;
    };
}