#pragma once

enum class PlayerColor
{
    White,
    Black,
    PlayerColor_MAX
};

class Player
{
private:
    PlayerColor color;
    long ticksLeft;
    int incrementAmount;
    int delaySecondsLeft;

public:
    explicit Player(PlayerColor color);
    int getMinutes();
    int getSeconds();
    int getTenths();
    int getDelayBar();

    bool isOutOfTime();
    bool isInDanger();
    bool isBlack();
    bool isWhite();

    void initialize(int minutes, int increment);
    void decrement();
    void increment();
    void incrementOneTick();
    void delayOrDecrement();
    void resetDelay();
};
