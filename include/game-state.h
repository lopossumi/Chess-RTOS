#pragma once

#include "enums.h"

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
        Game(TimerMode timerMode, int minutes, int increment);
        void reset(TimerMode timerMode, int minutes, int increment);

        bool buttonPressed(Button button);
        void endBlackTurn();
        void endWhiteTurn();
        void update();
        bool isBlinking();
        int getBlackDelayBar();
        int getWhiteDelayBar();
        bool isBlackInDanger();
        bool isWhiteInDanger();
    private:
        void selectNextMenuOption();
        void selectPreviousMenuOption();
        void commitMenuOption();
        void closeMenu();
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
        
        void resetBlackDelay();
        void resetWhiteDelay();
        void resetCurrentPlayerDelay();
};