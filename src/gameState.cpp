#include "gameState.hpp"
#include "player.hpp"
#include "enums.hpp"

GameState::GameState()
{
    blackTicksLeft = 0;
    whiteTicksLeft = 0;
    blackDelaySecondsLeft = 0;
    whiteDelaySecondsLeft = 0;

    isMenuOpen = false;
    isBlackTurn = true;
    isGameStarted = false;
    isPaused = false;
    isGameOver = false;
}

void GameState::initialize(int minutes, int increment)
{
    playtimeMinutes = minutes;
    incrementSeconds = increment;

    blackTicksLeft = minutes * 600l;
    whiteTicksLeft = minutes * 600l;
    blackDelaySecondsLeft = increment * 10;
    whiteDelaySecondsLeft = increment * 10;
}

int GameState::getBlackMinutes() { return blackTicksLeft / 600; }
int GameState::getWhiteMinutes() { return whiteTicksLeft / 600; }

int GameState::getBlackSeconds() { return (blackTicksLeft / 10) % 60; }
int GameState::getWhiteSeconds() { return (whiteTicksLeft / 10) % 60; }

int GameState::getBlackTenths() { return blackTicksLeft % 10; }
int GameState::getWhiteTenths() { return whiteTicksLeft % 10; }

int GameState::getBlackDelayBar() { return incrementSeconds == 0 ? 0 : (blackDelaySecondsLeft * 8) / incrementSeconds; }
int GameState::getWhiteDelayBar() { return incrementSeconds == 0 ? 0 : (whiteDelaySecondsLeft * 8) / incrementSeconds; }

bool GameState::isBlackOutOfTime() { return blackTicksLeft == 0; }
bool GameState::isWhiteOutOfTime() { return whiteTicksLeft == 0; }

bool GameState::isBlackInDanger() { return blackTicksLeft <= 100; }
bool GameState::isWhiteInDanger() { return whiteTicksLeft <= 100; }

bool GameState::isBlackTurnNow() { return isBlackTurn; }
bool GameState::isWhiteTurnNow() { return !isBlackTurn; }

void GameState::startGame() { isGameStarted = true; }
void GameState::pauseGame() { isPaused = true; }
void GameState::resumeGame() { isPaused = false; }
void GameState::endGame() { isGameOver = true; }

void GameState::decrementBlack() { blackTicksLeft--; }
void GameState::decrementWhite() { whiteTicksLeft--; }
void GameState::decrementCurrentPlayer() { isBlackTurn ? decrementBlack() : decrementWhite(); }

void GameState::incrementBlackOneTick() { blackTicksLeft++; }
void GameState::incrementWhiteOneTick() { whiteTicksLeft++; }
void GameState::incrementOtherOneTick() { isBlackTurn ? incrementWhiteOneTick() : incrementBlackOneTick(); }

void GameState::incrementBlack() { blackTicksLeft += incrementSeconds * 10; }
void GameState::incrementWhite() { whiteTicksLeft += incrementSeconds * 10; }
void GameState::incrementCurrentPlayer() { isBlackTurn ? incrementBlack() : incrementWhite(); }

void GameState::delayOrDecrementBlack()
{
    if (blackDelaySecondsLeft > 0)
    {
        blackDelaySecondsLeft--;
    }
    else
    {
        decrementBlack();
    }
}

void GameState::delayOrDecrementWhite()
{
    if (whiteDelaySecondsLeft > 0)
    {
        whiteDelaySecondsLeft--;
    }
    else
    {
        decrementWhite();
    }
}

void GameState::delayOrDecrementCurrentPlayer() { isBlackTurn ? delayOrDecrementBlack() : delayOrDecrementWhite(); }

void GameState::setCurrentPlayerToBlack() { isBlackTurn = true; }
void GameState::setCurrentPlayerToWhite() { isBlackTurn = false; }
void GameState::toggleCurrentPlayer() { isBlackTurn = !isBlackTurn; }

void GameState::resetBlackDelay() { blackDelaySecondsLeft = incrementSeconds; }
void GameState::resetWhiteDelay() { whiteDelaySecondsLeft = incrementSeconds; }
void GameState::resetCurrentPlayerDelay() { isBlackTurn ? resetBlackDelay() : resetWhiteDelay(); }

bool GameState::isPausedNow() { return isPaused; }

MenuItem GameState::getCurrentMenuItem() { return currentMenuItem; }
