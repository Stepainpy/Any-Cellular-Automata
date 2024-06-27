#include "guiworld.hpp"
#include <iostream>
#include <iterator>
#include <format>

#include "str_prefixs.hpp"
#include "parse.hpp"

bool operator<(const Color& lhs, const Color& rhs) {
    unsigned int lhsint = lhs.a << 24 | lhs.b << 16 | lhs.g << 8 | lhs.r;
    unsigned int rhsint = rhs.a << 24 | rhs.b << 16 | rhs.g << 8 | rhs.r;
    return lhsint < rhsint;
}

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

GuiWorld buildGuiWorld(std::list<Token>& lst, const std::string& settingPath) {
    const auto [width, height, fillChar, alphabet] = parse::stmt::world(lst);
    GuiSetting setting = parse::guiSet(settingPath);
    GuiWorld resultWorld(width, height, fillChar, setting);
    resultWorld.m_alphabet = alphabet;

    const auto aliases = parse::stmt::alias(lst);
    parse::stmt::rules(lst,  resultWorld, aliases);
    parse::stmt::setup(lst, &resultWorld, aliases);
    
    return resultWorld;
}

GuiWorld::GuiWorld(size_t w, size_t h, char fillCh, const GuiSetting& guiset)
    : width(w), height(h), N(w * h), m_guiSet(guiset) {
    m_world = std::make_unique<Color[]>(N);
    m_farSideWorld = std::make_unique<Color[]>(N);

    Color col = (!m_guiSet.foreConvert.contains(fillCh))
        ? Color{0, 0, 0, 0}
        : m_guiSet.foreConvert[fillCh];
    for (size_t i = 0; i < N; i++) {
        m_world[i] = col;
        m_farSideWorld[i] = col;
    }
}

void GuiWorld::update() {
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
            char c = getCell_FS(i);
            if (rule->applyRule(getCell(i), c, env, m_alphabet)) {
                setCell_FS(c, i);
                break;
            }
        }
    }

    std::copy(
        std::next(m_farSideWorld.get(), 0),
        std::next(m_farSideWorld.get(), N),
        std::next(m_world.get(), 0)
    );
}

void GuiWorld::display(bool viewIter) const {
    static size_t i = 0;
    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            DrawRectangle(
                (int)x * m_guiSet.pixelWidth,
                (int)y * m_guiSet.pixelHeight,
                m_guiSet.pixelWidth,
                m_guiSet.pixelHeight,
                m_world[x + width * y]
            );
        }
    }
    std::string message = std::format("Iter count: {}", i++);
    if (viewIter)
        DrawText(message.c_str(), 3, 3, 20, GREEN);
}

void GuiWorld::addRule(std::unique_ptr<Rule> rule) {
    m_rules.push_back(std::move(rule));
}

std::string GuiWorld::getAlphabet() const {
    return m_alphabet;
}

std::pair<int, int> GuiWorld::getWindowSize() const {
    return { width * m_guiSet.pixelWidth, height * m_guiSet.pixelHeight };
}

size_t GuiWorld::compressInWidth(size_t x) const {
    return x < width ? x : width - 1;
}

size_t GuiWorld::compressInHeight(size_t y) const {
    return y < height ? y : height - 1;
}

/*
Functions "getCell" auto wrap coordinate or index
in order to avoid out of range
*/

char GuiWorld::getCell(size_t x, size_t y) {
    return m_guiSet.backConvert[m_world[(y % height) * width + (x % width)]];
}

char GuiWorld::getCell(size_t i) {
    return m_guiSet.backConvert[m_world[i % N]];
}

char GuiWorld::getCell_FS(size_t x, size_t y) {
    return m_guiSet.backConvert[m_farSideWorld[(y % height) * width + (x % width)]];
}

char GuiWorld::getCell_FS(size_t i) {
    return m_guiSet.backConvert[m_farSideWorld[i % N]];
}

void GuiWorld::setCell(char c, size_t x, size_t y) {
    m_world[(y % height) * width + (x % width)] = m_guiSet.foreConvert[c];
}

void GuiWorld::setCell(char c, size_t i) {
    m_world[i % N] = m_guiSet.foreConvert[c];
}

void GuiWorld::setCell_FS(char c, size_t x, size_t y) {
    m_farSideWorld[(y % height) * width + (x % width)] = m_guiSet.foreConvert[c];
}

void GuiWorld::setCell_FS(char c, size_t i) {
    m_farSideWorld[i % N] = m_guiSet.foreConvert[c];
}