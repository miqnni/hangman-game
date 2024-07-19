#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

#define WORD_FILE_PATH R"(.\wordlist.txt)"
#define RESULTS_FILE_PATH R"(.\out\results.csv)"

constexpr int MIN_WORD_LENGTH{4};
constexpr int MAX_WORD_LENGTH{17};
constexpr int AVAILABLE_ATTEMPTS{8};
constexpr char BLANK_CHAR{'-'};

static_assert(AVAILABLE_ATTEMPTS > 0, "The user should have at least one available attempt.");
static_assert(MIN_WORD_LENGTH > 0, "The minimum word length must be a positive number.");
static_assert(MIN_WORD_LENGTH <= MAX_WORD_LENGTH, "The minimum word length cannot be greater than the maximum word length.");

enum class GameResult
{
    Defeat = -1,
    Unresolved, // = 0
    Victory,    // = 1
};

int main()
{
    try
    {
        // Open the file with words and
        // save the words with proper length to a vector.
        std::ifstream wordFile;
        std::ofstream resultFile;

        // Default mode (2nd parameter) for ifstream class open method: ios::in
        wordFile.open(WORD_FILE_PATH);

        bool resultFileExists{static_cast<bool>(std::ifstream(RESULTS_FILE_PATH))};

        // Default mode argument (2nd parameter) for ofstream class open method: ios::out
        resultFile.open(RESULTS_FILE_PATH, std::ios::out | std::ios::app);

        if (!wordFile.is_open())
            throw std::runtime_error("Unable to open input file.");
        if (!resultFile.is_open())
            throw std::runtime_error("Unable to open output file.");

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

        std::vector<char> guessedWord(soughtWord.size(), BLANK_CHAR);
        std::vector<char> incorrectGuesses(0);
        GameResult finalResult = GameResult::Unresolved;

        // Empty brace initializes the local variable
        // with zero (appropriate to the type).
        bool gameOver{};
        int roundNo{};

        while (!gameOver)
        {
            std::cout << std::endl
                      << "=== ROUND " << ++roundNo << " ===" << std::endl;
            // Print the current state of the game.
            // - guessed word
            std::cout << "Guessed: ";
            for (size_t i{0}; i < guessedWord.size(); i++)
            {
                std::cout << guessedWord[i];
            }
            std::cout << std::endl;

            // - incorrect guesses
            if (incorrectGuesses.size())
            {
                std::cout << "Incorrect guesses: ";
                for (size_t i{0}; i < incorrectGuesses.size(); i++)
                {
                    std::cout << incorrectGuesses[i] << ", ";
                }
                std::cout << std::endl;
            }

            // - hangman status
            std::cout << "Hangman status: ";
            for (size_t i{0}; i < AVAILABLE_ATTEMPTS; i++)
            {
                char barElement = (i < incorrectGuesses.size()) ? 'X' : 'O';
                std::cout << barElement;
            }
            std::cout << std::endl;

            // Ask the user about the letter and check if it is present in
            // the sought word. If it is, change the corresponding
            // vector element to the guessed char.
            char userGuess;
            std::cout << "Your letter: ";
            std::cin >> userGuess;

            // Check if the char from input is a letter.
            if ((userGuess < 'a' || userGuess > 'z') && (userGuess < 'A' || userGuess > 'Z'))
            {
                std::cout << "This is not a letter!" << std::endl;
                continue;
            }

            // Check if the letter has not been used before.
            bool letterNotUsed{true};
            for (size_t i{0}; i < incorrectGuesses.size(); i++)
            {
                if (tolower(incorrectGuesses[i]) == tolower(userGuess))
                {
                    letterNotUsed = false;
                    break;
                }
            }

            if (letterNotUsed)
            {
                for (size_t i{0}; i < guessedWord.size(); i++)
                {
                    if (guessedWord[i] == BLANK_CHAR)
                        continue;

                    if (tolower(guessedWord[i]) == tolower(userGuess))
                    {
                        letterNotUsed = false;
                        break;
                    }
                }
            }

            if (!letterNotUsed)
            {
                std::cout << "This letter has been used before." << std::endl;
                continue;
            }

            // Check if the guess is correct
            bool correctGuess{};
            for (size_t i{0}; i < soughtWord.size(); i++)
            {
                if (tolower(soughtWord[i]) == tolower(userGuess))
                {
                    guessedWord[i] = soughtWord[i];
                    correctGuess = true;
                }
            }
            if (!correctGuess)
            {
                incorrectGuesses.push_back(toupper(userGuess));
            }

            // Check if the game has ended.
            if (incorrectGuesses.size() >= AVAILABLE_ATTEMPTS)
            {
                gameOver = true;
                finalResult = GameResult::Defeat;
            }

            bool guessedFlag{true};
            for (size_t i{0}; i < guessedWord.size(); i++)
            {
                if (guessedWord[i] == BLANK_CHAR)
                {
                    guessedFlag = false;
                    break;
                }
            }
            if (guessedFlag)
            {
                gameOver = true;
                finalResult = GameResult::Victory;
            }
        }

        std::string finalResultTxt = (finalResult == GameResult::Victory ? "VICTORY" : "DEFEAT");
        std::cout << "Game over. Result: " << finalResultTxt << ". Solution: " << soughtWord << std::endl;

        // Results CSV export
        time_t currentTime;
        time(&currentTime);
        std::string currentTimeTxt{asctime(localtime(&currentTime))};
        // - remove newline characters from the current time string.
        currentTimeTxt.erase(std::remove(currentTimeTxt.begin(), currentTimeTxt.end(), '\n'), currentTimeTxt.end());

        if (!resultFileExists)
            resultFile << "current_time" << ";" << "solution" << ";" << "game_result" << ";" << "incorrect_guesses" << ";" << "available_attempts" << std::endl;

        resultFile << currentTimeTxt << ";" << soughtWord << ";" << finalResultTxt << ";" << incorrectGuesses.size() << ";" << AVAILABLE_ATTEMPTS << std::endl;

        wordFile.close();
        resultFile.close();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    system("pause");

    return 0;
}
