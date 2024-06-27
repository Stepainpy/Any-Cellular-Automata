#pragma once
#include <string>
#include <memory>
#include <format>
#include <tuple>
#include <list>
#include <map>

#include "world.hpp"
#include "conworld.hpp"
#include "guiworld.hpp"
#include "token.hpp"
#include "rules.hpp"

namespace parse {
    GuiSetting guiSet(const std::string& path);
    namespace stmt {
        std::tuple<size_t, size_t, char, std::string> world(std::list<Token>& lst);
        std::map<std::string, char> alias(std::list<Token>& lst);
        void rules(std::list<Token>& lst, World& world, const std::map<std::string, char>& aliases);
        void setup(std::list<Token>& lst, World* world, const std::map<std::string, char>& aliases);
    }
    namespace cmd {
        void cell   (std::list<Token>& lst, ConsoleWorld& world, const std::map<std::string, char>& aliases);
        void linex  (std::list<Token>& lst, ConsoleWorld& world, const std::map<std::string, char>& aliases);
        void liney  (std::list<Token>& lst, ConsoleWorld& world, const std::map<std::string, char>& aliases);
        void rect   (std::list<Token>& lst, ConsoleWorld& world, const std::map<std::string, char>& aliases);
        void pattern(std::list<Token>& lst, ConsoleWorld& world, const std::map<std::string, char>& aliases);
        void random (std::list<Token>& lst, ConsoleWorld& world, const std::map<std::string, char>& aliases);

        void cell   (std::list<Token>& lst, GuiWorld& world, const std::map<std::string, char>& aliases);
        void linex  (std::list<Token>& lst, GuiWorld& world, const std::map<std::string, char>& aliases);
        void liney  (std::list<Token>& lst, GuiWorld& world, const std::map<std::string, char>& aliases);
        void rect   (std::list<Token>& lst, GuiWorld& world, const std::map<std::string, char>& aliases);
        void pattern(std::list<Token>& lst, GuiWorld& world, const std::map<std::string, char>& aliases);
        void random (std::list<Token>& lst, GuiWorld& world, const std::map<std::string, char>& aliases);
    }
    namespace state {
        std::unique_ptr<CountRule>   count  (std::list<Token>& lst, const std::map<std::string, char>& aliases);
        std::unique_ptr<PatternRule> pattern(std::list<Token>& lst, const std::map<std::string, char>& aliases);
    }
}