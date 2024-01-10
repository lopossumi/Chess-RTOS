#pragma once

#include "enums.hpp"

class Game
{
    public:
        TimerMode timerMode;
        MenuItem menuItem;
        int playtimeMinutes;
        int incrementSeconds;

        bool isMenuOpen;
        bool isBlackTurn;
        bool isGameStarted;
        bool isPaused;
        bool isGameOver;
        bool updateHeader;

        long blackTicksLeft;
        long whiteTicksLeft;
        int blackDelayTicks;
        int whiteDelayTicks;

        Game();
        void initialize(TimerMode timerMode, int minutes, int increment);
        void buttonPressed(Button button);
        void incrementBlack();
        void incrementWhite();
        void update();
        bool isBlinking();
        int getBlackDelayBar();
        int getWhiteDelayBar();

    private:
        void selectNextMenuOption();
        void selectPreviousMenuOption();
        void commitMenuOption();
        void nextTimerMode();
        void previousTimerMode();
        void increaseMinutes();
        void decreaseMinutes();
        void increaseIncrement();
        void decreaseIncrement();

        void delayOrDecrementCurrentPlayer();
        void delayOrDecrementBlack();
        void delayOrDecrementWhite();

        void incrementCurrentPlayer();
        void incrementBlackOneTick();
        void incrementWhiteOneTick();
        void incrementOtherOneTick();
        
        bool isBlackInDanger();
        bool isWhiteInDanger();

        bool isBlackTurnNow();
        bool isWhiteTurnNow();

        void resetBlackDelay();
        void resetWhiteDelay();
        void resetCurrentPlayerDelay();
};