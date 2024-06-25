#include "parse.hpp"
#include <iostream>
#include <fstream>
#include <vector>

#include <nlohmann/json.hpp>
#include "str_prefixs.hpp"

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

GuiSetting parse::guiSet(const std::string& path) {
    using json = nlohmann::json;

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << ErrorPrefix << "Couldn't open the file '" << path << "'\n";
        std::exit(1);
    }
    std::string ErrorJsonPrefix = ErrorPrefix + path + ": ";

    json jSetting;
    try { jSetting = json::parse(file); }
    catch (const json::parse_error& err) {
        std::cerr << ErrorJsonPrefix << err.what() << std::endl;
        std::exit(1);
    }

    if (!jSetting.is_object()) {
        std::cerr << ErrorJsonPrefix << "json content must be object\n";
        std::exit(1);
    }

    GuiSetting result;

    if (!jSetting.contains("pixel") || !jSetting.contains("dict")) {
        std::cerr << ErrorJsonPrefix << "json must contains fields `pixel` and `dict`\n";
        std::exit(1);
    }
    json jPixel = jSetting["pixel"];
    json jDict = jSetting["dict"];

    if (!jPixel.is_object()) {
        std::cerr << ErrorJsonPrefix << "field `pixel` must be object\n";
        std::exit(1);
    }
    if (!jPixel.contains("width") || !jPixel.contains("height")) {
        std::cerr << ErrorJsonPrefix << "field `pixel` must containts fields `width` and `height`\n";
        std::exit(1);
    }
    if (!jPixel["width"].is_number_unsigned() || !jPixel["height"].is_number_unsigned()) {
        std::cerr << ErrorJsonPrefix << "fields `width` and `height` must be unsigned number\n";
        std::exit(1);
    }
    result.pixelWidth = jPixel["width"].get<int>();
    result.pixelHeight = jPixel["height"].get<int>();

    if (!jDict.is_object()) {
        std::cerr << ErrorJsonPrefix << "field `dict` must be object\n";
        std::exit(1);
    }
    for (const auto& compPair : jDict.items()) {
        if (compPair.key().length() > 1) {
            std::cerr << ErrorJsonPrefix << "compare char in json must be length 1\n";
            std::exit(1);
        }
        if (!compPair.value().is_array()) {
            std::cerr << ErrorJsonPrefix << "compare color in json must be array\n";
            std::exit(1);
        }
        char ch = compPair.key()[0];
        auto colorArray = compPair.value().get<std::vector<int>>();
        if (colorArray.size() != 3) {
            std::cerr << ErrorJsonPrefix << "compare color must be content 3 numbers\n";
            std::exit(1);
        }
        Color col = {
            (unsigned char)colorArray[0],
            (unsigned char)colorArray[1],
            (unsigned char)colorArray[2],
            255
        };

        result.foreConvert[ch] = col;
        result.backConvert[col] = ch;
    }

    return result;
}

std::tuple<size_t, size_t, char, std::string> parse::stmt::world(std::list<Token> &lst) {
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

    size_t albSize = std::abs(std::get<int>(alphabetSize.data));
    std::string alphabet;
    if (albSize == 0) {
        std::cerr << ErrorPrefix << "Alphabet size must be above zero\n";
        std::exit(1);
    }
    for (size_t i = 0; i < albSize; i++) {
        needTokenCount(lst, 1, "alphabet");
        Token albCh = pop(lst); albCh.mustBe(Token::Symbol);
        alphabet.push_back(std::get<char>(albCh.data));
    }

    return { width, height, std::get<char>(fillWorld.data), alphabet };
}

void parse::stmt::rules(std::list<Token>& lst, World& world) {
    needTokenCount(lst, 3, "rules");
    pop(lst).mustBe(Token::Phrase, "rules");
    Token ruleName  = pop(lst);  ruleName.mustBe(Token::Phrase, {"CountRule", "PatternRule"});
    Token ruleBlock = pop(lst); ruleBlock.mustBe(Token::Phrase, {"state", "end"});
    std::string ruleNameStr = ruleName.dataToStr();

    if (ruleBlock.dataToStr() == "state") {
        lst.push_front(ruleBlock);
        do {
            if (ruleNameStr == "CountRule")
                world.addRule(parse::state::count(lst));
            else if (ruleNameStr == "PatternRule")
                world.addRule(parse::state::pattern(lst));
        } while (needTokenCount(lst, 1, "rules") && (*lst.begin()).dataToStr() != "end");
        (void)pop(lst);  // drop 'end' of rules
    }
}

void parse::stmt::setup(std::list<Token>& lst, World* world) {
    needTokenCount(lst, 2, "setup");
    pop(lst).mustBe(Token::Phrase, "setup");
    const std::initializer_list<std::string> correctCmd = {"end", "cell", "linex", "liney", "rect", "pattern", "random"};
    Token commandTok = pop(lst); commandTok.mustBe(Token::Phrase, correctCmd);

    void (*conCmd)(std::list<Token>&, ConsoleWorld&) = nullptr;
    void (*guiCmd)(std::list<Token>&, GuiWorld&)     = nullptr;

    while (commandTok.dataToStr() != "end") {
        if (commandTok.dataToStr() == "cell") {
            conCmd = parse::cmd::cell;
            guiCmd = parse::cmd::cell;
        } else if (commandTok.dataToStr() == "linex") {
            conCmd = parse::cmd::linex;
            guiCmd = parse::cmd::linex;
        } else if (commandTok.dataToStr() == "liney") {
            conCmd = parse::cmd::liney;
            guiCmd = parse::cmd::liney;
        } else if (commandTok.dataToStr() == "rect") {
            conCmd = parse::cmd::rect;
            guiCmd = parse::cmd::rect;
        } else if (commandTok.dataToStr() == "pattern") {
            conCmd = parse::cmd::pattern;
            guiCmd = parse::cmd::pattern;
        } else if (commandTok.dataToStr() == "random") {
            conCmd = parse::cmd::random;
            guiCmd = parse::cmd::random;
        }

        if (ConsoleWorld* wrld = dynamic_cast<ConsoleWorld*>(world))
            conCmd(lst, *wrld);
        else if (GuiWorld* wrld = dynamic_cast<GuiWorld*>(world))
            guiCmd(lst, *wrld);
        else {
            std::cerr << ErrorPrefix << "unknown world type\n";
            std::exit(1);
        }

        needTokenCount(lst, 1, "setup");
        commandTok = pop(lst); commandTok.mustBe(Token::Phrase, correctCmd);
    }
}

std::unique_ptr<CountRule> parse::state::count(std::list<Token>& lst) {
    needTokenCount(lst, 5, "state");
    pop(lst).mustBe(Token::Phrase, "state");
    Token beforeChar = pop(lst); beforeChar.mustBe(Token::Symbol);
    pop(lst).mustBe(Token::Phrase, "to");
    Token afterChar = pop(lst); afterChar.mustBe(Token::Symbol);
    pop(lst).mustBe(Token::Phrase, "if");

    auto rule = std::make_unique<CountRule>(std::get<char>(beforeChar.data), std::get<char>(afterChar.data));

    bool isEmptyIf = true;
    while (needTokenCount(lst, 1, "if") && (*lst.begin()).dataToStr() != "end") {
        isEmptyIf = false;
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
    if (isEmptyIf)
        (void)pop(lst);  // pop 'end' of if block

    return rule;
}

std::unique_ptr<PatternRule> parse::state::pattern(std::list<Token>& lst) {
    needTokenCount(lst, 14, "state");
    pop(lst).mustBe(Token::Phrase, "state");
    Token beforeChar = pop(lst); beforeChar.mustBe(Token::Symbol);
    pop(lst).mustBe(Token::Phrase, "to");
    Token afterChar = pop(lst); afterChar.mustBe(Token::Symbol);
    pop(lst).mustBe(Token::Phrase, "if");
    
    std::string pattern;
    for (size_t i = 0; i < 8; i++) {
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
    pattern.insert(4, 1, std::get<char>(beforeChar.data));

    pop(lst).mustBe(Token::Phrase, "end");
    return std::make_unique<PatternRule>(std::get<char>(beforeChar.data), std::get<char>(afterChar.data), pattern);
}

void parse::cmd::cell(std::list<Token> &lst, ConsoleWorld &world) {
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

void parse::cmd::linex(std::list<Token> &lst, ConsoleWorld &world) {
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

void parse::cmd::liney(std::list<Token> &lst, ConsoleWorld &world) {
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

void parse::cmd::rect(std::list<Token> &lst, ConsoleWorld &world) {
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

void parse::cmd::pattern(std::list<Token> &lst, ConsoleWorld &world) {
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
        Token chTok = pop(lst);
        switch (chTok.type) {
            case Token::Symbol:
                pattern.push_back(std::get<char>(chTok.data));
            break;
            case Token::Phrase: {
                chTok.mustBe(Token::Phrase, "any");
                pattern.push_back('\0');
            } break;
            default: {
                std::cerr << ErrorPrefix << chTok.loc
                << "In pattern excepted \"symbol\" or `any`, but received `"
                << chTok.dataToStr() << "`\n";
                std::exit(1);
            } break;
        }
    }

    for (size_t row = 0; row < h; row++) {
        for (size_t i = 0; i < w; i++) {
            size_t ex = world.compressInWidth(x + i);
            size_t ey = world.compressInHeight(y + row);
            if (char c = pattern[row * w + i]) {
                world.getCell(ex, ey) = c;
                world.getCell_FS(ex, ey) = c;
            }
        }
    }
}

void parse::cmd::random(std::list<Token> &lst, ConsoleWorld &world) {
    needTokenCount(lst, 4, "random command");
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

void parse::cmd::cell(std::list<Token> &lst, GuiWorld &world) {
    needTokenCount(lst, 3, "cell command");
    Token chTok = pop(lst); chTok.mustBe(Token::Symbol);
    Token xTok  = pop(lst);  xTok.mustBe(Token::Number);
    Token yTok  = pop(lst);  yTok.mustBe(Token::Number);

    size_t x = world.compressInWidth(std::abs(std::get<int>(xTok.data)));
    size_t y = world.compressInHeight(std::abs(std::get<int>(yTok.data)));

    char c = std::get<char>(chTok.data);
    world.setCell(c, x, y);
    world.setCell_FS(c, x, y);
}

void parse::cmd::linex(std::list<Token> &lst, GuiWorld &world) {
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
        world.setCell(c, tx, y);
        world.setCell_FS(c, tx, y);
    }
}

void parse::cmd::liney(std::list<Token> &lst, GuiWorld &world) {
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
        world.setCell(c, x, ty);
        world.setCell_FS(c, x, ty);
    }
}

void parse::cmd::rect(std::list<Token> &lst, GuiWorld &world) {
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
            world.setCell(c, tx, ty);
            world.setCell_FS(c, tx, ty);
        }
    }
}

void parse::cmd::pattern(std::list<Token> &lst, GuiWorld &world) {
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
        Token chTok = pop(lst);
        switch (chTok.type) {
            case Token::Symbol:
                pattern.push_back(std::get<char>(chTok.data));
            break;
            case Token::Phrase: {
                chTok.mustBe(Token::Phrase, "any");
                pattern.push_back('\0');
            } break;
            default: {
                std::cerr << ErrorPrefix << chTok.loc
                << "In pattern excepted \"symbol\" or `any`, but received `"
                << chTok.dataToStr() << "`\n";
                std::exit(1);
            } break;
        }
    }

    for (size_t row = 0; row < h; row++) {
        for (size_t i = 0; i < w; i++) {
            size_t ex = world.compressInWidth(x + i);
            size_t ey = world.compressInHeight(y + row);
            if (char c = pattern[row * w + i]) {
                world.setCell(c, ex, ey);
                world.setCell_FS(c, ex, ey);
            }
        }
    }
}

void parse::cmd::random(std::list<Token> &lst, GuiWorld &world) {
    needTokenCount(lst, 4, "random command");
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

    size_t ex = world.compressInWidth(x + w - 1);
    size_t ey = world.compressInHeight(y + h - 1);

    std::string alphabet = world.getAlphabet();
    for (size_t ty = y; ty <= ey; ty++) {
        for (size_t tx = x; tx <= ex; tx++) {
            char c = alphabet[rand() % alphabet.size()];
            world.setCell(c, tx, ty);
            world.setCell_FS(c, tx, ty);
        }
    }
}