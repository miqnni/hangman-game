#include <raylib.h>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <iomanip>

#include "game.h"
#include "gameresult.h"
#include "settings.h"

/*
TODO:
- put every .h and .cpp file except for main.cpp to a separate "src/model" directory
- random word from a .txt file
- export game results to a .csv file
- display remaining attempts in a different way
*/

int main()
{
    try
    {
        // Raylib settings
        Color bgColor = Color{33, 32, 30, 255};
        Color fgColor = Color{255, 255, 255, 255};
        const int screenWidth = 800;
        const int screenHeight = 600;
        InitWindow(screenWidth, screenHeight, "Hangman Game");
        SetTargetFPS(60);

        const std::string soughtWord{"constientiousness"};

        Settings settings{screenWidth, screenHeight, '-', bgColor, fgColor, 24, 20, 20, 8};
        Game game(settings, soughtWord);

        while (!WindowShouldClose())
        {
            game.draw();
            if (int userGuessId{GetCharPressed()})
            {
                game.update(static_cast<char>(userGuessId));
            }
        }
        CloseWindow();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
