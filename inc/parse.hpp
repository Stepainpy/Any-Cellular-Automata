#pragma once
#include <string>
#include <memory>
#include <format>
#include <tuple>
#include <list>

#include "world.hpp"
#include "conworld.hpp"
#include "guiworld.hpp"
#include "token.hpp"
#include "rules.hpp"

Token pop(std::list<Token>& lst);
bool needTokenCount(const std::list<Token>& lst, size_t count, const std::string& stmt);

namespace parse {
    GuiSetting guiSet(const std::string& path);
    namespace stmt {
        std::tuple<size_t, size_t, char, std::string> world(std::list<Token>& lst);
        void rules(std::list<Token>& lst, World& world);
        void setup(std::list<Token>& lst, World* world);
    }
    namespace cmd {
        void cell   (std::list<Token>& lst, ConsoleWorld& world);
        void linex  (std::list<Token>& lst, ConsoleWorld& world);
        void liney  (std::list<Token>& lst, ConsoleWorld& world);
        void rect   (std::list<Token>& lst, ConsoleWorld& world);
        void pattern(std::list<Token>& lst, ConsoleWorld& world);
        void random (std::list<Token>& lst, ConsoleWorld& world);

        void cell   (std::list<Token>& lst, GuiWorld& world);
        void linex  (std::list<Token>& lst, GuiWorld& world);
        void liney  (std::list<Token>& lst, GuiWorld& world);
        void rect   (std::list<Token>& lst, GuiWorld& world);
        void pattern(std::list<Token>& lst, GuiWorld& world);
        void random (std::list<Token>& lst, GuiWorld& world);
    }
    namespace state {
        std::unique_ptr<CountRule> count(std::list<Token>& lst);
        std::unique_ptr<PatternRule> pattern(std::list<Token>& lst);
    }
}