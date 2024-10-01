#include "conworld.hpp"
#include <iostream>
#include <sstream>

#include "str_prefixs.hpp"
#include "parse.hpp"

const std::pair<long long, long long> deltas[8] = {
    { -1, -1 },  // NW
    {  0, -1 },  // N
    {  1, -1 },  // NE
    { -1,  0 },  // W
    {  1,  0 },  // E
    { -1,  1 },  // SW
    {  0,  1 },  // S
    {  1,  1 }   // SE
};

ConsoleWorld buildConsoleWorld(std::list<Token>& lst) {
    const auto [width, height, fillChar, alphabet] = parse::stmt::world(lst);
    ConsoleWorld resultWorld(width, height, fillChar);
    resultWorld.m_alphabet = alphabet;

    const auto aliases = parse::stmt::alias(lst);
    parse::stmt::rules(lst,  resultWorld, aliases);
    parse::stmt::setup(lst, &resultWorld, aliases);
    
    return resultWorld;
}

ConsoleWorld::ConsoleWorld(size_t w, size_t h, char fillCh)
    : width(w), height(h), N(w * h) {
    std::ostringstream oss;
    for (size_t j = 0; j < h; j++)
        oss << std::string(w, fillCh) << '\n';
    m_farSideWorld = m_world = oss.str();
    m_world.pop_back();
    m_farSideWorld.pop_back();
}

void ConsoleWorld::update() {
    using ll = long long;
    static const auto mod = [](ll a, ll b) -> ll {
        return ((a % b) + b) % b;
    };

    #pragma omp parallel for
    for (size_t i = 0; i < N; i++) {
        std::string env;
        for (const auto& delta : deltas) {
            ll x = (i % N) % width;
            ll y = (i % N) / width;
            x = (size_t)mod(x + delta.first, width);
            y = (size_t)mod(y + delta.second, height);
            env += getCell(x, y);
        }
        for (const auto& rule : m_rules) {
            if (rule->applyRule(getCell(i), getCell_FS(i), env, m_alphabet))
                break;
        }
    }
    m_world = m_farSideWorld;
}

void ConsoleWorld::display() const {
    // return to start pos (left top) and display world
    std::cout << "\x1b[H" << m_world;
}

void ConsoleWorld::addRule(std::unique_ptr<Rule> rule) {
    m_rules.push_back(std::move(rule));
}

std::string ConsoleWorld::getAlphabet() const {
    return m_alphabet;
}

size_t ConsoleWorld::compressInWidth(size_t x) const {
    return x < width ? x : width - 1;
}

size_t ConsoleWorld::compressInHeight(size_t y) const {
    return y < height ? y : height - 1;
}

/*
Functions "getCell" auto wrap coordinate or index
in order to avoid out of range
*/

char& ConsoleWorld::getCell(size_t x, size_t y) {
    return m_world[(y % height) * (width + 1) + (x % width)];
}

char& ConsoleWorld::getCell(size_t i) {
    return m_world[(i % N) + (i % N) / width];
}

char& ConsoleWorld::getCell_FS(size_t x, size_t y) {
    return m_farSideWorld[(y % height) * (width + 1) + (x % width)];
}

char& ConsoleWorld::getCell_FS(size_t i) {
    return m_farSideWorld[(i % N) + (i % N) / width];
}