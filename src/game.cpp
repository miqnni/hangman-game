#include "game.h"

Game::Game(Settings settings, std::string soughtWord)
    : soughtWord(soughtWord), roundNo(1), over(false), result(GameResult::Unresolved), settings(settings)
{
    for (size_t i = 0; i < soughtWord.size(); i++)
        guessedWord.push_back(settings.blankChar);
}

Game::~Game()
{
}

void Game::update(char userGuess)
{
    if (over)
        return;

    // Validate guess
    // Check if the char from input is a letter.
    if ((userGuess < 'a' || userGuess > 'z') && (userGuess < 'A' || userGuess > 'Z'))
    {
        return;
    }

    if (letterUsed(userGuess))
    {
        return;
    }

    if (!validateGuess(userGuess))
    {
        incorrectGuesses.push_back(userGuess);
    }

    // Is the game over?
    checkGameStatus();

    roundNo++;
}

void Game::draw()
{
    BeginDrawing();
    ClearBackground(settings.bgColor);

    std::string roundNoMsg{"=== Round " + std::to_string(roundNo) + " ==="};
    std::string guessedMsg{"Guessed: "};
    std::string incorrectMsg{"Incorrect: "};
    std::string hangmanStatusMsg{"Hangman status: "};

    for (auto el : guessedWord)
    {
        guessedMsg += el;
    }

    for (auto el : incorrectGuesses)
    {
        incorrectMsg += el;
        incorrectMsg += ", ";
    }

    for (int i{0}; i < settings.availableAttempts; i++)
    {
        char barElement = (i < incorrectGuesses.size()) ? 'X' : 'O';
        hangmanStatusMsg += barElement;
    }

    const char *roundNoCStr{roundNoMsg.c_str()};
    const char *guessedCStr{guessedMsg.c_str()};
    const char *incorrectCStr{incorrectMsg.c_str()};
    const char *hangmanStatusCStr{hangmanStatusMsg.c_str()};

    DrawText(roundNoCStr, settings.offsetX, settings.offsetY + 0 * settings.fontSize, settings.fontSize, settings.fgColor);
    DrawText(guessedCStr, settings.offsetX, settings.offsetY + 2 * settings.fontSize, settings.fontSize, settings.fgColor);
    DrawText(incorrectCStr, settings.offsetX, settings.offsetY + 4 * settings.fontSize, settings.fontSize, settings.fgColor);
    DrawText(hangmanStatusCStr, settings.offsetX, settings.offsetY + 6 * settings.fontSize, settings.fontSize, settings.fgColor);

    // After guessing
    if (over)
    {
        std::string finalResultTxt = (result == GameResult::Victory ? "VICTORY" : "DEFEAT");
        finalResultTxt += " (solution: " + soughtWord;
        finalResultTxt += ")";
        const char *finalResultCStr = finalResultTxt.c_str();
        // std::cout << "Game over. Result: " << finalResultTxt << ". Solution: " << soughtWord << std::endl;
        DrawText(finalResultCStr, settings.offsetX, settings.offsetY + 8 * settings.fontSize, settings.fontSize, settings.fgColor);
    }

    EndDrawing();
}

// Check if the letter has been used before.
bool Game::letterUsed(char userGuess)
{
    for (size_t i{0}; i < incorrectGuesses.size(); i++)
    {
        if (tolower(incorrectGuesses[i]) == tolower(userGuess))
        {
            return true;
        }
    }

    for (size_t i{0}; i < guessedWord.size(); i++)
    {
        if (guessedWord[i] == settings.blankChar)
            continue;

        if (tolower(guessedWord[i]) == tolower(userGuess))
        {
            return true;
        }
    }

    return false;
}

// Check if the guess is correct and update the guessedWord status.
bool Game::validateGuess(char userGuess)
{
    bool res{};
    for (size_t i{0}; i < soughtWord.size(); i++)
    {
        if (tolower(soughtWord[i]) == tolower(userGuess))
        {
            guessedWord[i] = soughtWord[i];
            res = true;
        }
    }

    return res;
}

// Check if the game has ended.
void Game::checkGameStatus()
{
    if (incorrectGuesses.size() >= settings.availableAttempts)
    {
        over = true;
        result = GameResult::Defeat;
    }

    bool guessedFlag{true};
    for (size_t i{0}; i < guessedWord.size(); i++)
    {
        if (guessedWord[i] == settings.blankChar)
        {
            guessedFlag = false;
            break;
        }
    }
    if (guessedFlag)
    {
        over = true;
        result = GameResult::Victory;
    }
}
