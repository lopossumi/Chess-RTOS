#include "gameState.h"
#include "enums.hpp"

GameState::GameState() { }

void GameState::initialize(TimerMode timerMode, int minutes, int increment)
{
    this->timerMode = timerMode;
    playtimeMinutes = minutes;
    incrementSeconds = increment;

    menuItem = MenuItem::Mode;
    isMenuOpen = false;
    isBlackTurn = true;
    isGameStarted = false;
    isPaused = false;
    isGameOver = false;
}

long GameState::getBlackTicksLeft() { return blackTicksLeft; }
long GameState::getWhiteTicksLeft() { return whiteTicksLeft; }

int GameState::getBlackDelayBar() { return incrementSeconds == 0 ? 0 : (blackDelayTicks * 8) / incrementSeconds; }
int GameState::getWhiteDelayBar() { return incrementSeconds == 0 ? 0 : (whiteDelayTicks * 8) / incrementSeconds; }

bool GameState::isBlackOutOfTime() { return blackTicksLeft == 0; }
bool GameState::isWhiteOutOfTime() { return whiteTicksLeft == 0; }

bool GameState::isBlackInDanger() { return blackTicksLeft <= 100; }
bool GameState::isWhiteInDanger() { return whiteTicksLeft <= 100; }

bool GameState::isBlackTurnNow() { return isBlackTurn; }
bool GameState::isWhiteTurnNow() { return !isBlackTurn; }

void GameState::decrementBlack() 
{ 
    if (blackTicksLeft > 0)
    {
        blackTicksLeft--; 
    }
    else
    {
        isGameOver = true;
    }
}
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
    if (blackDelayTicks > 0)
    {
        blackDelayTicks--;
    }
    else
    {
        decrementBlack();
    }
}

void GameState::delayOrDecrementWhite()
{
    if (whiteDelayTicks > 0)
    {
        whiteDelayTicks--;
    }
    else
    {
        decrementWhite();
    }
}

void GameState::update() 
{ 
    isBlackTurn 
        ? delayOrDecrementBlack() 
        : delayOrDecrementWhite();

    if(timerMode == TimerMode::Hourglass) { incrementOtherOneTick(); }

    if(isBlackOutOfTime() || isWhiteOutOfTime()) { isGameOver = true; }
}

void GameState::resetBlackDelay() { blackDelayTicks = incrementSeconds; }
void GameState::resetWhiteDelay() { whiteDelayTicks = incrementSeconds; }
void GameState::resetCurrentPlayerDelay() { isBlackTurn ? resetBlackDelay() : resetWhiteDelay(); }

void GameState::previousTimerMode()
{
    timerMode = (timerMode <= (TimerMode)0) ? (TimerMode)((int)TimerMode::TimerMode_MAX - 1) : (TimerMode)((int)timerMode - 1);
}

void GameState::nextTimerMode()
{
    timerMode = (timerMode >= (TimerMode)((int)TimerMode::TimerMode_MAX - 1)) ? (TimerMode)0 : (TimerMode)((int)timerMode + 1);
}

bool GameState::isBlinking()
{
    return isGameStarted && (isPaused || (!isGameOver && (isBlackInDanger() || isWhiteInDanger())));
}

void GameState::decreaseMinutes()
{
    switch (playtimeMinutes)
    {
        case 61 ... 90: playtimeMinutes = 60;   break;
        case 31 ... 60: playtimeMinutes -= 15;  break;
        case 6  ... 30: playtimeMinutes -= 5;   break;
        case 2  ...  5: playtimeMinutes -= 1;   break;
        
        default:
            break;
    }
}

void GameState::increaseMinutes()
{
    switch(playtimeMinutes)
    {
        case 1  ...  4: playtimeMinutes += 1;   break;
        case 5  ... 29: playtimeMinutes += 5;   break;
        case 30 ... 59: playtimeMinutes += 15;  break;
        case 60 ... 90: playtimeMinutes = 90;   break;
        
        default:
            break;
    }
}


void GameState::decreaseIncrement()
{
    incrementSeconds = (incrementSeconds > 0) ? incrementSeconds - 5 : 0;
}

void GameState::increaseIncrement()
{
    incrementSeconds = (incrementSeconds < 60) ? incrementSeconds + 5 : 60;
}

void GameState::selectNextMenuOption()
{
    switch (menuItem)
    {
        case MenuItem::Mode:        nextTimerMode();        break;
        case MenuItem::Minutes:     increaseMinutes();      break;
        case MenuItem::Increment:   increaseIncrement();    break;
        default:
            break;
    }
}

void GameState::selectPreviousMenuOption()
{    
    switch (menuItem)
    {
        case MenuItem::Mode:        previousTimerMode();    break;
        case MenuItem::Minutes:     decreaseMinutes();      break;
        case MenuItem::Increment:   decreaseIncrement();    break;
        default:
            break;
    }
}

void GameState::commitMenuOption()
{
    switch (menuItem)
    {
    case MenuItem::Mode:
        menuItem = MenuItem::Minutes;
        break;

    case MenuItem::Minutes:
        menuItem = MenuItem::Increment;
        break;

    case MenuItem::Increment:
        menuItem = MenuItem::Mode;
        isMenuOpen = false;

        blackTicksLeft = playtimeMinutes * 600l;
        whiteTicksLeft = playtimeMinutes * 600l;
        blackDelayTicks = incrementSeconds;
        whiteDelayTicks = incrementSeconds;
        break;

    default:
        break;
    }
}

void GameState::buttonPressed(Button button)
{
    if(isMenuOpen){
        switch (button)
        {
            case Button::White:     selectNextMenuOption();         break;
            case Button::Black:     selectPreviousMenuOption();     break;
            case Button::Select:    commitMenuOption();             break;

            default:
                break;    
        }
        return;
    }

    if(!isGameStarted){
        switch (button)
        {
            case Button::White:
            case Button::Black:
                isGameStarted = true;
                isBlackTurn = button == Button::White;
                break;

            default:
                break;
        }
        return;
    }

    if(isPaused){
        switch (button)
        {
            case Button::Select:
                isPaused = false;
                break;

            default:
                break;
        }
        return;
    }

    if(isGameOver)
    {
        switch (button)
        {
        case Button::Select:
            initialize(timerMode, playtimeMinutes, incrementSeconds);
            break;

        default:
            break;
        }
        return;
    }

    // Default: game is running
    switch (button)
    {
    case Button::White:
        if (!isBlackTurn)
        {
            incrementWhite();
            isBlackTurn = true;
        }
        break;

    case Button::Black:
        if (isBlackTurn)
        {
            incrementBlack();
            isBlackTurn = false;
        }
        break;

    case Button::Select:
        isPaused = true;
        break;

    default:
        break;
    }
}