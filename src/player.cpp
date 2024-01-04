#include "player.hpp"
#include "enums.hpp"

Player::Player(PlayerColor color)
{
    this->color = color;
    ticksLeft = 0;
    incrementAmount = 0;
    delay = 0;
}

int Player::getMinutes() { return ticksLeft / 600; }
int Player::getSeconds() { return (ticksLeft / 10) % 60; }
int Player::getTenths() { return ticksLeft % 10; }
int Player::getDelayBar()
{
    // The return value is calculated from a fraction of delay / incrementAmount and scaled to 0 - 7.
    if (incrementAmount == 0)
    {
        return 0;
    }
    return (delay * 8) / incrementAmount;
}

bool Player::isOutOfTime() { return ticksLeft == 0; }
bool Player::isInDanger() { return ticksLeft <= 100; }
bool Player::isBlack() { return color == PlayerColor::Black; }
bool Player::isWhite() { return color == PlayerColor::White; }

void Player::initialize(int minutes, int increment)
{
    ticksLeft = minutes * 600l;
    incrementAmount = increment * 10;
    delay = incrementAmount;
}

void Player::decrement()
{
    if (ticksLeft > 0)
    {
        ticksLeft--;
    }
}

void Player::increment()
{
    ticksLeft += incrementAmount;
}

void Player::incrementOneTick()
{
    ticksLeft++;
}

void Player::delayOrDecrement()
{
    if (delay > 0)
    {
        delay--;
    }
    else
    {
        decrement();
    }
}

void Player::resetDelay()
{
    delay = incrementAmount;
}
