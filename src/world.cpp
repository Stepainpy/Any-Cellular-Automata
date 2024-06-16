#include "world.hpp"
#include <iostream>
#include <sstream>
#include <format>

#include "str_prefixs.hpp"

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

const std::set<size_t> allMayNeighbors = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };

// Support functions

long long mod(long long a, long long b) {
    return ((a % b) + b) % b;
}

std::string top(const std::list<Token>& lst) {
    return (*lst.begin()).dataToStr();
}

Token pop(std::list<Token>& lst) {
    Token t = *lst.begin();
    lst.erase(lst.begin());
    return t;
}

bool needTokenCount(const std::list<Token>& lst, size_t count, const std::string& stmt) {
    if (lst.size() < count) {
        std::cerr << ErrorPrefix << std::format("Insufficient data for {} statement\n", stmt);
        std::exit(1);
    }
    return true;
}

// end supfunc

std::unique_ptr<CountRule> parseCountRuleState(std::list<Token>& lst) {
    needTokenCount(lst, 5, "state");
    pop(lst).mustBe(Token::Phrase, "state");
    Token beforeChar = pop(lst); beforeChar.mustBe(Token::Symbol);
    pop(lst).mustBe(Token::Phrase, "to");
    Token afterChar = pop(lst); afterChar.mustBe(Token::Symbol);
    pop(lst).mustBe(Token::Phrase, "if");

    auto rule = std::make_unique<CountRule>(std::get<char>(beforeChar.data), std::get<char>(afterChar.data));

    while (needTokenCount(lst, 1, "if") && top(lst) != "end") {
        needTokenCount(lst, 3, "may");
        Token mayChar = pop(lst); mayChar.mustBe(Token::Symbol);
        Token mayType = pop(lst); mayType.mustBe(Token::Phrase, {"may", "mayall"});
        Token maybeEnd = pop(lst);
        bool isMayAll = mayType.dataToStr() == "mayall";

        std::set<size_t> neighCnts;
        if (isMayAll)
            neighCnts = allMayNeighbors;

        while (maybeEnd.type == Token::Number) {
            size_t tmpNum = std::abs(std::get<int>(maybeEnd.data));
            if (!isMayAll)
                neighCnts.insert(tmpNum);
            else
                neighCnts.extract(tmpNum);

            needTokenCount(lst, 1, "may numbers");
            maybeEnd = pop(lst);
        }

        rule->addSubRule(std::get<char>(mayChar.data), neighCnts);
        if (maybeEnd.dataToStr() == "end")
            break;
        lst.push_front(maybeEnd);
    }

    return rule;
}

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
    Token ruleName  = pop(lst);  ruleName.mustBe(Token::Phrase, {"CountRule"});
    Token ruleBlock = pop(lst); ruleBlock.mustBe(Token::Phrase, {"state", "end"});

    if (ruleName.dataToStr() == "CountRule") {
        if (ruleBlock.dataToStr() == "state") {
            lst.push_front(ruleBlock);
            do {
                resultWorld.addRule(parseCountRuleState(lst));
            } while (needTokenCount(lst, 1, "rules") && top(lst) != "end");
            (void)pop(lst);  // drop 'end' of rules
        }
    }

    /* Parse setup statement */
    needTokenCount(lst, 2, "setup");
    pop(lst).mustBe(Token::Phrase, "setup");
    Token commandTok = pop(lst); commandTok.mustBe(Token::Phrase,
        {"end", "cell", "linex", "liney", "rect", "pattern"});
    
    while (commandTok.dataToStr() != "end") {
        if (commandTok.dataToStr() == "cell") {
            needTokenCount(lst, 3, "cell command");
            Token chTok = pop(lst); chTok.mustBe(Token::Symbol);
            Token xTok  = pop(lst);  xTok.mustBe(Token::Number);
            Token yTok  = pop(lst);  yTok.mustBe(Token::Number);

            size_t x = resultWorld.compressInWidth(std::abs(std::get<int>(xTok.data)));
            size_t y = resultWorld.compressInHeight(std::abs(std::get<int>(yTok.data)));

            char c = std::get<char>(chTok.data);
            resultWorld.getCell(x, y) = c;
            resultWorld.getCell_FS(x, y) = c;
        } else if (commandTok.dataToStr() == "linex") {
            needTokenCount(lst, 4, "linex command");
            Token chTok = pop(lst); chTok.mustBe(Token::Symbol);
            Token x0Tok = pop(lst); x0Tok.mustBe(Token::Number);
            Token yTok  = pop(lst);  yTok.mustBe(Token::Number);
            Token x1Tok = pop(lst); x1Tok.mustBe(Token::Number);

            size_t x0 = resultWorld.compressInWidth(std::abs(std::get<int>(x0Tok.data)));
            size_t x1 = resultWorld.compressInWidth(std::abs(std::get<int>(x1Tok.data)));
            size_t y = resultWorld.compressInHeight(std::abs(std::get<int>(yTok.data)));

            if (x0 > x1)
                std::swap(x0, x1);

            for (size_t tx = x0; tx <= x1; tx++) {
                char c = std::get<char>(chTok.data);
                resultWorld.getCell(tx, y) = c;
                resultWorld.getCell_FS(tx, y) = c;
            }
        } else if (commandTok.dataToStr() == "liney") {
            needTokenCount(lst, 4, "liney command");
            Token chTok = pop(lst); chTok.mustBe(Token::Symbol);
            Token xTok  = pop(lst);  xTok.mustBe(Token::Number);
            Token y0Tok = pop(lst); y0Tok.mustBe(Token::Number);
            Token y1Tok = pop(lst); y1Tok.mustBe(Token::Number);

            size_t x  = resultWorld.compressInWidth(std::abs(std::get<int>(xTok.data)));
            size_t y0 = resultWorld.compressInHeight(std::abs(std::get<int>(y0Tok.data)));
            size_t y1 = resultWorld.compressInHeight(std::abs(std::get<int>(y1Tok.data)));

            if (y0 > y1)
                std::swap(y0, y1);

            for (size_t ty = y0; ty <= y1; ty++) {
                char c = std::get<char>(chTok.data);
                resultWorld.getCell(x, ty) = c;
                resultWorld.getCell_FS(x, ty) = c;
            }
        } else if (commandTok.dataToStr() == "rect") {
            needTokenCount(lst, 5, "rect command");
            Token chTok = pop(lst); chTok.mustBe(Token::Symbol);
            Token  xTok = pop(lst);  xTok.mustBe(Token::Number);
            Token  yTok = pop(lst);  yTok.mustBe(Token::Number);
            Token  wTok = pop(lst);  wTok.mustBe(Token::Number);
            Token  hTok = pop(lst);  hTok.mustBe(Token::Number);

            size_t x = resultWorld.compressInWidth(std::abs(std::get<int>(xTok.data)));
            size_t y = resultWorld.compressInHeight(std::abs(std::get<int>(yTok.data)));
            size_t w = std::abs(std::get<int>(wTok.data));
            size_t h = std::abs(std::get<int>(hTok.data));

            if (w == 0 || h == 0) {
                std::cerr << ErrorPrefix << "Rect size for command must be above zero\n";
                std::exit(1);
            }

            size_t ex = resultWorld.compressInWidth(x + w - 1);
            size_t ey = resultWorld.compressInHeight(y + h - 1);

            for (size_t ty = y; ty <= ey; ty++) {
                for (size_t tx = x; tx <= ex; tx++) {
                    char c = std::get<char>(chTok.data);
                    resultWorld.getCell(tx, ty) = c;
                    resultWorld.getCell_FS(tx, ty) = c;
                }
            }
        } else if (commandTok.dataToStr() == "pattern") {
            needTokenCount(lst, 4, "pattern command");
            Token xTok = pop(lst); xTok.mustBe(Token::Number);
            Token yTok = pop(lst); yTok.mustBe(Token::Number);
            Token wTok = pop(lst); wTok.mustBe(Token::Number);
            Token hTok = pop(lst); hTok.mustBe(Token::Number);

            size_t x = resultWorld.compressInWidth(std::abs(std::get<int>(xTok.data)));
            size_t y = resultWorld.compressInHeight(std::abs(std::get<int>(yTok.data)));
            size_t w = std::abs(std::get<int>(wTok.data));
            size_t h = std::abs(std::get<int>(hTok.data));

            if (w == 0 || h == 0) {
                std::cerr << ErrorPrefix << "Rect size for command must be above zero\n";
                std::exit(1);
            }

            size_t patternLength = w * h;
            std::string pattern;
            needTokenCount(lst, patternLength, "pattern command chars");
            for (size_t i = 0; i < patternLength; i++) {
                Token chTok = pop(lst); chTok.mustBe(Token::Symbol);
                pattern.push_back(std::get<char>(chTok.data));
            }

            for (size_t row = 0; row < h; row++) {
                for (size_t i = 0; i < w; i++) {
                    size_t ex = resultWorld.compressInWidth(x + i);
                    size_t ey = resultWorld.compressInHeight(y + row);
                    char c = pattern[row * w + i];
                    resultWorld.getCell(ex, ey) = c;
                    resultWorld.getCell_FS(ex, ey) = c;
                }
            }
        }

        needTokenCount(lst, 1, "setup");
        commandTok = pop(lst); commandTok.mustBe(Token::Phrase,
            {"end", "cell", "linex", "liney", "rect", "pattern"});
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
    #pragma omp parallel for
    for (size_t i = 0; i < N; i++) {
        std::string env;
        for (const auto& delta : deltas) {
            long long x = (i % N) % width;
            long long y = (i % N) / width;
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

void World::display() {
    static size_t i = 0;
    // return to start pos (left top) and display world
    std::cout << "\e[H" << m_world <<
    "(Q - exit) Iter count: " << i++;
}

void World::addRule(std::unique_ptr<Rule> rule) {
    m_rules.push_back(std::move(rule));
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

size_t World::compressInWidth(size_t x) {
    return x < width ? x : width - 1;
}

size_t World::compressInHeight(size_t y) {
    return y < height ? y : height - 1;
}