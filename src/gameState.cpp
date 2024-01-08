#include "gameState.hpp"
#include "player.hpp"

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
