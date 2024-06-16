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
    void display();

    char& getCell(size_t x, size_t y);
    char& getCell(size_t i);

    void addRule(std::unique_ptr<Rule> rule);

private:
    World(size_t w, size_t h, char fillCh);
    friend World buildWorld(std::list<Token>& lst);
    
    char& getCell_FS(size_t x, size_t y);
    char& getCell_FS(size_t i);

    size_t compressInWidth(size_t x);
    size_t compressInHeight(size_t y);

private:
    size_t width, height, N;
    std::string m_world;
    std::string m_farSideWorld;
    std::string m_alphabet;
    std::vector<std::unique_ptr<Rule>> m_rules;
};

World buildWorld(std::list<Token>& lst);