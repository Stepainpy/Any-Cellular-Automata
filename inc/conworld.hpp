#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <list>

#include "world.hpp"

#include "rules.hpp"
#include "token.hpp"

class ConsoleWorld : public World {
public:
    void update();
    void display(bool viewIter) const;

    char& getCell(size_t x, size_t y);
    char& getCell(size_t i);
    char& getCell_FS(size_t x, size_t y);
    char& getCell_FS(size_t i);

    size_t compressInWidth(size_t x) const;
    size_t compressInHeight(size_t y) const;

    void addRule(std::unique_ptr<Rule> rule) override;

    std::string getAlphabet() const;

private:
    ConsoleWorld(size_t w, size_t h, char fillCh);
    friend ConsoleWorld buildConsoleWorld(std::list<Token>& lst);

private:
    size_t width, height, N;
    std::string m_world;
    std::string m_farSideWorld;
    std::string m_alphabet;
    std::vector<std::unique_ptr<Rule>> m_rules;
};

ConsoleWorld buildConsoleWorld(std::list<Token>& lst);