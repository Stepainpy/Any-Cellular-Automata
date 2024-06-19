#include "world.hpp"
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

World buildWorld(std::list<Token>& lst) {
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

    World resultWorld(width, height, std::get<char>(fillWorld.data));

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

World::World(size_t w, size_t h, char fillCh)
    : width(w), height(h), N(w * h) {
    std::ostringstream oss;
    for (size_t j = 0; j < h; j++)
        oss << std::string(w, fillCh) << '\n';
    m_farSideWorld = m_world = oss.str();
}

void World::update() {
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

void World::display() const {
    static size_t i = 0;
    // return to start pos (left top) and display world
    std::cout << "\e[H" << m_world <<
    "(Esc - exit) Iter count: " << i++;
}

void World::addRule(std::unique_ptr<Rule> rule) {
    m_rules.push_back(std::move(rule));
}

std::string World::getAlphabet() const {
    return m_alphabet;
}

size_t World::compressInWidth(size_t x) const {
    return x < width ? x : width - 1;
}

size_t World::compressInHeight(size_t y) const {
    return y < height ? y : height - 1;
}

/*
Functions "getCell" auto wrap coordinate or index
in order to avoid out of range
*/

char& World::getCell(size_t x, size_t y) {
    return m_world[(y % height) * (width + 1) + (x % width)];
}

char& World::getCell(size_t i) {
    return m_world[(i % N) + (i % N) / width];
}

char& World::getCell_FS(size_t x, size_t y) {
    return m_farSideWorld[(y % height) * (width + 1) + (x % width)];
}

char& World::getCell_FS(size_t i) {
    return m_farSideWorld[(i % N) + (i % N) / width];
}