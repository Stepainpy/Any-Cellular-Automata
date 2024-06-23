#pragma once
#include <string>
#include <memory>
#include <format>
#include <list>

#include "conworld.hpp"
#include "token.hpp"
#include "rules.hpp"

Token pop(std::list<Token>& lst);
bool needTokenCount(const std::list<Token>& lst, size_t count, const std::string& stmt);

namespace parse {
    namespace cmd {
        void cell   (std::list<Token>& lst, ConsoleWorld& world);
        void linex  (std::list<Token>& lst, ConsoleWorld& world);
        void liney  (std::list<Token>& lst, ConsoleWorld& world);
        void rect   (std::list<Token>& lst, ConsoleWorld& world);
        void pattern(std::list<Token>& lst, ConsoleWorld& world);
        void random (std::list<Token>& lst, ConsoleWorld& world);
    }
    namespace state {
        std::unique_ptr<CountRule> count(std::list<Token>& lst);
        std::unique_ptr<PatternRule> pattern(std::list<Token>& lst);
    }
}