#pragma once

#include "enums.hpp"

class GameState
{
    public:
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

        MenuItem menuItem;
        TimerMode timerMode;

        void incrementBlack();
        void incrementWhite();

        GameState();
        void initialize(TimerMode timerMode, int minutes, int increment);

        long getBlackTicksLeft();
        long getWhiteTicksLeft();

        int getBlackDelayBar();       
        int getWhiteDelayBar();

        bool isBlackOutOfTime();
        bool isWhiteOutOfTime();

        bool isBlackInDanger();
        bool isWhiteInDanger();

        bool isBlackTurnNow();
        bool isWhiteTurnNow();

        void startGame();
        void pauseGame();
        void resumeGame();
        void endGame();

        void decrementBlack();
        void decrementWhite();
        void decrementCurrentPlayer();

        void incrementCurrentPlayer();

        void incrementBlackOneTick();
        void incrementWhiteOneTick();
        void incrementOtherOneTick();

        void delayOrDecrementBlack();
        void delayOrDecrementWhite();
        void delayOrDecrementCurrentPlayer();

        void resetBlackDelay();
        void resetWhiteDelay();
        void resetCurrentPlayerDelay();

        void setCurrentPlayerToBlack();
        void setCurrentPlayerToWhite();
        void toggleCurrentPlayer();

        bool isPausedNow();
        
        bool isMenuOpenNow();
        void openMenu();
        void closeMenu();

        MenuItem getMenuItem();
        TimerMode getTimerMode();

        int getPlayTimeMinutes();
        int getIncrementSeconds();

        void setTimerMode();
        void nextTimerMode();
        void previousTimerMode();
};