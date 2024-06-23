#pragma once
#include <map>
#include <raylib/raylib.h>

bool operator<(const Color& lhs, const Color& rhs);

struct GuiSetting {
    int pixelWidth;
    int pixelHeight;
    std::map<char, Color> foreConvert;
    std::map<Color, char> backConvert;
};