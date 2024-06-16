#include "token.hpp"
#include <algorithm>
#include <iterator>
#include <iostream>
#include <format>

#include "str_prefixs.hpp"

std::ostream& operator<<(std::ostream& out, const Location& loc) {
    return out << loc.path << ':' << loc.line << ':' << loc.column << ": ";
}

std::string Token::dataToStr() const {
    switch (type) {
        case Symbol:
            return std::string(1, std::get<char>(data));
        case Number:
            return std::to_string(std::get<int>(data));
        case Phrase:
            return std::get<std::string>(data);
        default:
            return "";
    }
}

bool Token::mustBe(TokenType _type) {
    if (type != _type) {
        std::string strType;
        switch (_type) {
            case Symbol: { strType = "symbol"; } break;
            case Number: { strType = "number"; } break;
            case Phrase: { strType = "word"  ; } break;
        }
        std::cerr << ErrorPrefix << loc << std::format("Expected \"{}\", but received `{}`\n", strType, dataToStr());
        std::exit(1);
    }
    return true;
}

bool Token::mustBe(TokenType _type, const std::string& str) {
    if (type != _type) {
        std::string strType;
        switch (_type) {
            case Symbol: { strType = "symbol"; } break;
            case Number: { strType = "number"; } break;
            case Phrase: { strType = "word"  ; } break;
        }
        std::cerr << ErrorPrefix << loc << std::format("Expected \"{}\", but received `{}`\n", strType, dataToStr());
        std::exit(1);
    } else if (_type == Phrase && dataToStr() != str) {
        std::cerr << ErrorPrefix << loc << std::format("Expected `{}`, but received `{}`\n", str, dataToStr());
        std::exit(1);
    }
    return true;
}

bool Token::mustBe(TokenType _type, const std::initializer_list<std::string> &strs) {
    if (type != _type) {
        std::string strType;
        switch (_type) {
            case Symbol: { strType = "symbol"; } break;
            case Number: { strType = "number"; } break;
            case Phrase: { strType = "word"  ; } break;
        }
        std::cerr << ErrorPrefix << loc << std::format("Expected \"{}\", but received `{}`\n", strType, dataToStr());
        std::exit(1);
    } else if (_type == Phrase && std::none_of(strs.begin(), strs.end(), [this](const std::string& s) { return this->dataToStr() == s; })) {
        std::cerr << ErrorPrefix << loc << "Excepted `";
        std::copy(strs.begin(), strs.end() - 1, std::ostream_iterator<std::string>(std::cerr, "` or `"));
        std::cerr << *(strs.end() - 1) << std::format("`, but received `{}`\n", dataToStr());
        std::exit(1);
    }
    return true;
}