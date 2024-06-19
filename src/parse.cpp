#include "parse.hpp"
#include <iostream>

#include "str_prefixs.hpp"

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

std::unique_ptr<CountRule> parse::state::count(std::list<Token>& lst) {
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
        Token mayType = pop(lst); mayType.mustBe(Token::Phrase, {"may", "nomay"});
        Token maybeEnd = pop(lst);
        bool isNoMay = mayType.dataToStr() == "nomay";

        std::set<size_t> neighCnts;
        if (isNoMay)
            neighCnts = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };

        while (maybeEnd.type == Token::Number) {
            size_t tmpNum = std::abs(std::get<int>(maybeEnd.data));
            if (!isNoMay)
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

std::unique_ptr<PatternRule> parse::state::pattern(std::list<Token>& lst) {
    needTokenCount(lst, 15, "state");
    pop(lst).mustBe(Token::Phrase, "state");
    Token beforeChar = pop(lst); beforeChar.mustBe(Token::Symbol);
    pop(lst).mustBe(Token::Phrase, "to");
    Token afterChar = pop(lst); afterChar.mustBe(Token::Symbol);
    pop(lst).mustBe(Token::Phrase, "if");
    
    std::string pattern;
    for (size_t i = 0; i < 9; i++) {
        Token patPt = pop(lst);

        switch (patPt.type) {
            case Token::Symbol:
                pattern.push_back(std::get<char>(patPt.data));
            break;
            case Token::Phrase: {
                patPt.mustBe(Token::Phrase, "any");
                pattern.push_back('\0');
            } break;
            default: {
                std::cerr << ErrorPrefix << patPt.loc
                << "In pattern excepted \"symbol\" or `any`, but received `"
                << patPt.dataToStr() << "`\n";
                std::exit(1);
            } break;
        }
    }

    pop(lst).mustBe(Token::Phrase, "end");
    return std::make_unique<PatternRule>(std::get<char>(beforeChar.data), std::get<char>(afterChar.data), pattern);
}

void parse::cmd::cell(std::list<Token> &lst, World &world) {
    needTokenCount(lst, 3, "cell command");
    Token chTok = pop(lst); chTok.mustBe(Token::Symbol);
    Token xTok  = pop(lst);  xTok.mustBe(Token::Number);
    Token yTok  = pop(lst);  yTok.mustBe(Token::Number);

    size_t x = world.compressInWidth(std::abs(std::get<int>(xTok.data)));
    size_t y = world.compressInHeight(std::abs(std::get<int>(yTok.data)));

    char c = std::get<char>(chTok.data);
    world.getCell(x, y) = c;
    world.getCell_FS(x, y) = c;
}

void parse::cmd::linex(std::list<Token> &lst, World &world) {
    needTokenCount(lst, 4, "linex command");
    Token chTok = pop(lst); chTok.mustBe(Token::Symbol);
    Token x0Tok = pop(lst); x0Tok.mustBe(Token::Number);
    Token yTok  = pop(lst);  yTok.mustBe(Token::Number);
    Token x1Tok = pop(lst); x1Tok.mustBe(Token::Number);

    size_t x0 = world.compressInWidth(std::abs(std::get<int>(x0Tok.data)));
    size_t x1 = world.compressInWidth(std::abs(std::get<int>(x1Tok.data)));
    size_t y = world.compressInHeight(std::abs(std::get<int>(yTok.data)));

    if (x0 > x1)
        std::swap(x0, x1);

    for (size_t tx = x0; tx <= x1; tx++) {
        char c = std::get<char>(chTok.data);
        world.getCell(tx, y) = c;
        world.getCell_FS(tx, y) = c;
    }
}

void parse::cmd::liney(std::list<Token> &lst, World &world) {
    needTokenCount(lst, 4, "liney command");
    Token chTok = pop(lst); chTok.mustBe(Token::Symbol);
    Token xTok  = pop(lst);  xTok.mustBe(Token::Number);
    Token y0Tok = pop(lst); y0Tok.mustBe(Token::Number);
    Token y1Tok = pop(lst); y1Tok.mustBe(Token::Number);

    size_t x  = world.compressInWidth(std::abs(std::get<int>(xTok.data)));
    size_t y0 = world.compressInHeight(std::abs(std::get<int>(y0Tok.data)));
    size_t y1 = world.compressInHeight(std::abs(std::get<int>(y1Tok.data)));

    if (y0 > y1)
        std::swap(y0, y1);

    for (size_t ty = y0; ty <= y1; ty++) {
        char c = std::get<char>(chTok.data);
        world.getCell(x, ty) = c;
        world.getCell_FS(x, ty) = c;
    }
}

void parse::cmd::rect(std::list<Token> &lst, World &world) {
    needTokenCount(lst, 5, "rect command");
    Token chTok = pop(lst); chTok.mustBe(Token::Symbol);
    Token  xTok = pop(lst);  xTok.mustBe(Token::Number);
    Token  yTok = pop(lst);  yTok.mustBe(Token::Number);
    Token  wTok = pop(lst);  wTok.mustBe(Token::Number);
    Token  hTok = pop(lst);  hTok.mustBe(Token::Number);

    size_t x = world.compressInWidth(std::abs(std::get<int>(xTok.data)));
    size_t y = world.compressInHeight(std::abs(std::get<int>(yTok.data)));
    size_t w = std::abs(std::get<int>(wTok.data));
    size_t h = std::abs(std::get<int>(hTok.data));

    if (w == 0 || h == 0) {
        std::cerr << ErrorPrefix << "Rect size for command must be above zero\n";
        std::exit(1);
    }

    size_t ex = world.compressInWidth(x + w - 1);
    size_t ey = world.compressInHeight(y + h - 1);

    for (size_t ty = y; ty <= ey; ty++) {
        for (size_t tx = x; tx <= ex; tx++) {
            char c = std::get<char>(chTok.data);
            world.getCell(tx, ty) = c;
            world.getCell_FS(tx, ty) = c;
        }
    }
}

void parse::cmd::pattern(std::list<Token> &lst, World &world) {
    needTokenCount(lst, 4, "pattern command");
    Token xTok = pop(lst); xTok.mustBe(Token::Number);
    Token yTok = pop(lst); yTok.mustBe(Token::Number);
    Token wTok = pop(lst); wTok.mustBe(Token::Number);
    Token hTok = pop(lst); hTok.mustBe(Token::Number);

    size_t x = world.compressInWidth(std::abs(std::get<int>(xTok.data)));
    size_t y = world.compressInHeight(std::abs(std::get<int>(yTok.data)));
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
            size_t ex = world.compressInWidth(x + i);
            size_t ey = world.compressInHeight(y + row);
            char c = pattern[row * w + i];
            world.getCell(ex, ey) = c;
            world.getCell_FS(ex, ey) = c;
        }
    }
}

void parse::cmd::random(std::list<Token> &lst, World &world) {
    needTokenCount(lst, 4, "random command");
    Token  xTok = pop(lst);  xTok.mustBe(Token::Number);
    Token  yTok = pop(lst);  yTok.mustBe(Token::Number);
    Token  wTok = pop(lst);  wTok.mustBe(Token::Number);
    Token  hTok = pop(lst);  hTok.mustBe(Token::Number);

    size_t x = world.compressInWidth(std::abs(std::get<int>(xTok.data)));
    size_t y = world.compressInHeight(std::abs(std::get<int>(yTok.data)));
    size_t w = std::abs(std::get<int>(wTok.data));
    size_t h = std::abs(std::get<int>(hTok.data));

    if (w == 0 || h == 0) {
        std::cerr << ErrorPrefix << "Rect size for command must be above zero\n";
        std::exit(1);
    }

    size_t ex = world.compressInWidth(x + w - 1);
    size_t ey = world.compressInHeight(y + h - 1);

    std::string alphabet = world.getAlphabet();
    for (size_t ty = y; ty <= ey; ty++) {
        for (size_t tx = x; tx <= ex; tx++) {
            char c = alphabet[rand() % alphabet.size()];
            world.getCell(tx, ty) = c;
            world.getCell_FS(tx, ty) = c;
        }
    }
}