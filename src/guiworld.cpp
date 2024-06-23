#include "guiworld.hpp"
#include <iostream>
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
    /* Parse world statement */
    needTokenCount(lst, 5, "world");
    pop(lst).mustBe(Token::Phrase, "world");
    Token widthWorld   = pop(lst);   widthWorld.mustBe(Token::Number);
    Token heightWorld  = pop(lst);  heightWorld.mustBe(Token::Number);
    Token fillWorld    = pop(lst);    fillWorld.mustBe(Token::Symbol);
    Token alphabetSize = pop(lst); alphabetSize.mustBe(Token::Number);

    size_t width  = std::abs(std::get<int>(widthWorld.data));
    size_t height = std::abs(std::get<int>(heightWorld.data));
    if (width == 0 || height == 0) {
        std::cerr << ErrorPrefix << "World size must be above zero\n";
        std::exit(1);
    }

    GuiSetting setting = parse::guiSet(settingPath);
    GuiWorld resultWorld(width, height, std::get<char>(fillWorld.data), setting);

    size_t albSize = std::abs(std::get<int>(alphabetSize.data));
    if (albSize == 0) {
        std::cerr << ErrorPrefix << "Alphabet size must be above zero\n";
        std::exit(1);
    }
    for (size_t i = 0; i < albSize; i++) {
        needTokenCount(lst, 1, "alphabet");
        Token albCh = pop(lst); albCh.mustBe(Token::Symbol);
        resultWorld.m_alphabet.push_back(std::get<char>(albCh.data));
    }

    /* Parse rules statement */
    needTokenCount(lst, 3, "rules");
    pop(lst).mustBe(Token::Phrase, "rules");
    Token ruleName  = pop(lst);  ruleName.mustBe(Token::Phrase, {"CountRule", "PatternRule"});
    Token ruleBlock = pop(lst); ruleBlock.mustBe(Token::Phrase, {"state", "end"});
    std::string ruleNameStr = ruleName.dataToStr();

    if (ruleBlock.dataToStr() == "state") {
        lst.push_front(ruleBlock);
        do {
            if (ruleNameStr == "CountRule")
                resultWorld.addRule(parse::state::count(lst));
            else if (ruleNameStr == "PatternRule")
                resultWorld.addRule(parse::state::pattern(lst));
        } while (needTokenCount(lst, 1, "rules") && (*lst.begin()).dataToStr() != "end");
        (void)pop(lst);  // drop 'end' of rules
    }

    /* Parse setup statement */
    needTokenCount(lst, 2, "setup");
    pop(lst).mustBe(Token::Phrase, "setup");
    const std::initializer_list<std::string> correctCmd = {"end", "cell", "linex", "liney", "rect", "pattern", "random"};
    Token commandTok = pop(lst); commandTok.mustBe(Token::Phrase, correctCmd);
    
    while (commandTok.dataToStr() != "end") {
        if (commandTok.dataToStr() == "cell") {
            parse::cmd::cell(lst, resultWorld);
        } else if (commandTok.dataToStr() == "linex") {
            parse::cmd::linex(lst, resultWorld);
        } else if (commandTok.dataToStr() == "liney") {
            parse::cmd::liney(lst, resultWorld);
        } else if (commandTok.dataToStr() == "rect") {
            parse::cmd::rect(lst, resultWorld);
        } else if (commandTok.dataToStr() == "pattern") {
            parse::cmd::pattern(lst, resultWorld);
        } else if (commandTok.dataToStr() == "random") {
            parse::cmd::random(lst, resultWorld);
        }

        needTokenCount(lst, 1, "setup");
        commandTok = pop(lst); commandTok.mustBe(Token::Phrase, correctCmd);
    }
    
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

    for (size_t i = 0; i < N; i++) {
        m_world[i] = m_farSideWorld[i];
    }
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