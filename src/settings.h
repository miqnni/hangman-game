#pragma once
#include <raylib.h>

class Settings
{
public:
    int windowWidth;
    int windowHeight;
    char blankChar;
    Color bgColor;
    Color fgColor;
    int fontSize;
    int offsetX;
    int offsetY;
    int availableAttempts;
};