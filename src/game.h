#include <string>
#include <vector>
#include "gameresult.h"
#include "settings.h"

class Game
{
private:
    std::string soughtWord;
    std::vector<char> guessedWord;
    std::vector<char> incorrectGuesses;
    int roundNo;
    bool over;
    GameResult result;
    Settings settings;

public:
    Game(Settings settings, std::string soughtWord);

    ~Game();

    void update(char userGuess);

    void draw();

    bool isOver() { return over; }

    bool letterUsed(char userGuess);

    bool validateGuess(char userGuess);

    void checkGameStatus();
};
