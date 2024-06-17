#include "lexer.hpp"
#include <iostream>
#include <locale>

#include "str_prefixs.hpp"

Lexer::Lexer(const std::string& filePath) : m_file(filePath), m_globalLoc(1, 1, filePath) {
    if (!m_file.is_open()) {
        std::cerr << ErrorPrefix << "Couldn't open the file '" << filePath << "'\n";
        std::exit(1);
    }
}

void Lexer::skip() {
    while (std::isspace(m_file.peek()) || m_file.peek() == ';') {
        char c = m_file.get();
        switch (c) {
            case ' ' : case '\t': m_globalLoc.column++; break;
            case '\r': m_globalLoc.column = 1; break;
            case '\v': m_globalLoc.line++; break;
            case '\n': {
                m_globalLoc.line++;
                m_globalLoc.column = 1;
            } break;
            case ';': {
                while (m_file.peek() != '\n')
                    c = m_file.get();
            } break;
        }
    }
}

void Lexer::appendToken(std::list<Token>& lst, const std::string& word, Location loc) {
    try {
        size_t ncount = 0;
        int number = std::stoi(word, &ncount);

        if (word.size() == ncount)
            lst.emplace_back(number, Token::Number, loc);
        else
            lst.emplace_back(word, Token::Phrase, loc);
    } catch (...) {
        if (word.size() == 3 && word[0] == '\'' && word[2] == '\'')
            lst.emplace_back(word[1], Token::Symbol, loc);
        else
            lst.emplace_back(word, Token::Phrase, loc);
    }
}

std::list<Token> Lexer::parse() {
    if (!m_file.is_open()) {
        std::cerr << ErrorPrefix << "Reopening tokenized file\n";
        std::exit(1);
    }

    std::list<Token> result;
    std::string buffer;

    while (m_file.peek() != -1) {
        skip();

        buffer.push_back(m_file.get());
        if (*(buffer.end() - 1) == '\'') {
            bool isEsc = false;
            while (m_file.peek() != -1) {
                char c = m_file.get();
                buffer.push_back(c);
                if (c == '\\') {
                    buffer.pop_back();
                    isEsc = true;
                    continue;
                }
                if (c == '\'' && !isEsc)
                    break;
                isEsc = false;
            }
        }

        if (std::isspace(m_file.peek()) || m_file.peek() == -1) {
            appendToken(result, buffer, m_globalLoc);
            m_globalLoc.column += buffer.size();
            buffer.clear();
        }
    }

    m_file.close();
    return result;
}