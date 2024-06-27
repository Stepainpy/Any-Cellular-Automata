#pragma once
#include <initializer_list>
#include <ostream>
#include <variant>
#include <string>

struct Location {
    size_t line;
    size_t column;
    std::string path;

    Location(size_t l, size_t c, const std::string& p)
        : line(l), column(c), path(p) {}
};

std::ostream& operator<<(std::ostream& out, const Location& loc);

struct Token {
    enum TokenType {
        Symbol,
        Number,
        Phrase
    };

    std::variant<char, int, std::string> data;
    TokenType type;
    Location loc;

    std::string dataToStr() const;
    bool mustBe(TokenType _type) const;
    bool mustBe(TokenType _type, const std::string& str) const;
    bool mustBe(const std::initializer_list<TokenType>& _types) const;
    bool mustBe(TokenType _type, const std::initializer_list<std::string>& str) const;
};