#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <list>

#include "world.hpp"

#include <raylib/raylib.h>
#include "rules.hpp"
#include "token.hpp"
#include "guiset.hpp"

class GuiWorld : public World {
public:
    void update();
    void display(bool viewIters) const;

    char getCell(size_t x, size_t y);
    char getCell(size_t i);
    char getCell_FS(size_t x, size_t y);
    char getCell_FS(size_t i);

    void setCell(char c, size_t x, size_t y);
    void setCell(char c, size_t i);
    void setCell_FS(char c, size_t x, size_t y);
    void setCell_FS(char c, size_t i);

    size_t compressInWidth(size_t x) const;
    size_t compressInHeight(size_t y) const;

    void addRule(std::unique_ptr<Rule> rule) override;

    std::string getAlphabet() const;
    std::pair<int, int> getWindowSize() const;

private:
    GuiWorld(size_t w, size_t h, char fillCh, const GuiSetting& guiset);
    friend GuiWorld buildGuiWorld(std::list<Token>& lst, const std::string& settingPath);

private:
    size_t width, height, N;
    GuiSetting m_guiSet;
    std::string m_alphabet;
    std::unique_ptr<Color[]> m_world;
    std::unique_ptr<Color[]> m_farSideWorld;
    std::vector<std::unique_ptr<Rule>> m_rules;
};

GuiWorld buildGuiWorld(std::list<Token>& lst, const std::string& settingPath);