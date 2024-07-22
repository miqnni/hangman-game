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

#define WORD_FILE_PATH R"(wordlist.txt)"
#define RESULTS_FILE_PATH R"(results.csv)"

constexpr int MIN_WORD_LENGTH{4};
constexpr int MAX_WORD_LENGTH{17};
static_assert(MIN_WORD_LENGTH > 0, "The minimum word length must be a positive number.");
static_assert(MIN_WORD_LENGTH <= MAX_WORD_LENGTH, "The minimum word length cannot be greater than the maximum word length.");

int main()
{
    try
    {
        // RANDOM WORD (start) **************************************************
        // Open the file with words and
        // save the words with proper length to a vector.
        std::ifstream wordFile;

        // Default mode (2nd parameter) for ifstream class open method: ios::in
        wordFile.open(WORD_FILE_PATH);
        if (!wordFile.is_open())
            throw std::runtime_error("Unable to open input file.");

        // Open the CSV file for game results.
        std::ofstream resultFile;
        bool resultFileExists{static_cast<bool>(std::ifstream(RESULTS_FILE_PATH))};

        // Default mode argument (2nd parameter) for ofstream class open method: ios::out
        resultFile.open(RESULTS_FILE_PATH, std::ios::out | std::ios::app);
        if (!resultFile.is_open())
            throw std::runtime_error("Unable to open output file.");

        // If the CSV file has just been created, add proper headings.
        if (!resultFileExists)
            resultFile << "current_time" << ";" << "solution" << ";" << "game_result" << ";" << "incorrect_guesses" << ";" << "available_attempts" << std::endl;

        std::vector<std::string> availableWords;

        std::string currentWord;

        while (getline(wordFile, currentWord))
        {
            if (currentWord.size() >= MIN_WORD_LENGTH && currentWord.size() <= MAX_WORD_LENGTH)
                availableWords.push_back(currentWord);
        }

        if (!availableWords.size())
            throw std::runtime_error("There are no words matching the criteria. Consider changing the values of MIN_WORD_LENGTH or MAX_WORD_LENGTH.");

        // Get the random word to be the answer.
        srand(time(NULL));
        int randomWordID = rand() % availableWords.size();
        const std::string soughtWord{availableWords[randomWordID]};
        // RANDOM WORD (end) **************************************************

        // Raylib settings
        Color bgColor = Color{33, 32, 30, 255};
        Color fgColor = Color{255, 255, 255, 255};
        const int screenWidth = 800;
        const int screenHeight = 600;
        InitWindow(screenWidth, screenHeight, "Hangman Game");
        SetTargetFPS(60);

        Settings settings{screenWidth, screenHeight, '-', bgColor, fgColor, 24, 20, 20, 8};
        Game game(settings, soughtWord);

        bool exportedCSV{};
        while (!WindowShouldClose())
        {
            game.draw();
            if (int userGuessId{GetCharPressed()})
            {
                game.update(static_cast<char>(userGuessId));
                if (game.isOver() && !exportedCSV)
                {
                    exportedCSV = true;

                    std::time_t currentTime;
                    std::time(&currentTime);
                    std::tm currentTimeTmStruct = *std::localtime(&currentTime);

                    std::string finalResultTxt = (game.getResult() == GameResult::Victory ? "VICTORY" : "DEFEAT");
                    std::cout << "Game over. Result: " << finalResultTxt << ". Solution: " << soughtWord << std::endl;

                    resultFile << std::put_time(&currentTimeTmStruct, "%Y-%m-%d %H:%M:%S") << ";" << soughtWord << ";" << finalResultTxt << ";" << game.getIncorrectGuesses().size() << ";" << settings.availableAttempts << std::endl;
                }
            }
        }
        CloseWindow();
        wordFile.close();
        resultFile.close();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
