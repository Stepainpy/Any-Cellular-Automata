#pragma once
#include <cstdint>
#include <utility>
#include <string>
#include <vector>
#include <memory>
#include <list>

#include "rules.hpp"
#include "token.hpp"

class World {
public:
    void update();
    void display() const;

    char& getCell(size_t x, size_t y);
    char& getCell(size_t i);

    char& getCell_FS(size_t x, size_t y);
    char& getCell_FS(size_t i);

    size_t compressInWidth(size_t x) const;
    size_t compressInHeight(size_t y) const;

    void addRule(std::unique_ptr<Rule> rule);

    std::string getAlphabet() const;

private:
    World(size_t w, size_t h, char fillCh);
    friend World buildWorld(std::list<Token>& lst);

private:
    size_t width, height, N;
    std::string m_world;
    std::string m_farSideWorld;
    std::string m_alphabet;
    std::vector<std::unique_ptr<Rule>> m_rules;
};

World buildWorld(std::list<Token>& lst);