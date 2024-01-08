#pragma once

class GameState
{
    private:
        long blackTicksLeft;
        long whiteTicksLeft;
        int blackDelaySecondsLeft;
        int whiteDelaySecondsLeft;

        int playtimeMinutes;
        int incrementSeconds;

        bool isMenuOpen;
        bool isBlackTurn;
        bool isGameStarted;
        bool isPaused;
        bool isGameOver;

    public:
        GameState();
};