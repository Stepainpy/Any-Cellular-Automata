#pragma once
#include <fstream>
#include <string>
#include <list>

#include "token.hpp"

class Lexer {
public:
    Lexer(const std::string& filePath);
    ~Lexer() { done(); }
    std::list<Token> parse();

private:
    void done() {
        if (m_file.is_open())
            m_file.close();
    }
    void skip();
    void appendToken(std::list<Token>& lst, const std::string& word, Location loc);

private:
    std::ifstream m_file;
    Location m_globalLoc;
};