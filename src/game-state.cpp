#include "game-state.h"
#include "enums.h"

Game::Game(TimerMode timerMode, int minutes, int increment)
{
    reset(timerMode, minutes, increment);
}

void Game::reset(TimerMode timerMode, int minutes, int increment)
{
    this->timerMode = timerMode;
    playtimeMinutes = minutes;
    incrementSeconds = increment;

    menuItem = MenuItem::Mode;
    isMenuOpen = true;
    isBlackTurn = true;
    isGameStarted = false;
    isPaused = false;
    isGameOver = false;
    updateHeader = true;

    blackTicksLeft = playtimeMinutes * 600l;
    whiteTicksLeft = playtimeMinutes * 600l;
    blackDelayTicks = incrementSeconds * 10;
    whiteDelayTicks = incrementSeconds * 10;
}

int Game::getBlackDelayBar() { return timerMode == TimerMode::SimpleDelay ? (blackDelayTicks * 8) / (incrementSeconds * 10) : 0; }
int Game::getWhiteDelayBar() { return timerMode == TimerMode::SimpleDelay ? (whiteDelayTicks * 8) / (incrementSeconds * 10) : 0; }

bool Game::isBlackInDanger() { return blackTicksLeft <= 100; }
bool Game::isWhiteInDanger() { return whiteTicksLeft <= 100; }

void Game::incrementBlackOneTick() { blackTicksLeft++; }
void Game::incrementWhiteOneTick() { whiteTicksLeft++; }
void Game::incrementOtherOneTick() { isBlackTurn ? incrementWhiteOneTick() : incrementBlackOneTick(); }

void Game::endBlackTurn() 
{
    if(timerMode == TimerMode::Fischer)
    {
        blackTicksLeft += incrementSeconds * 10;
    }
    if(timerMode == TimerMode::SimpleDelay)
    {
        resetBlackDelay();
    }
    isBlackTurn = false;
}

void Game::endWhiteTurn() 
{ 
    if(timerMode == TimerMode::Fischer)
    {
        whiteTicksLeft += incrementSeconds * 10;
    }
    if(timerMode == TimerMode::SimpleDelay)
    {
        resetWhiteDelay();
    }
    isBlackTurn = true;
}

void Game::incrementCurrentPlayer() { isBlackTurn ? endBlackTurn() : endWhiteTurn(); }

void Game::delayOrDecrementCurrentPlayer()
{
    isBlackTurn ? delayOrDecrementBlack() : delayOrDecrementWhite();
}

void Game::delayOrDecrementBlack()
{
    if (blackDelayTicks > 0)
    { 
        blackDelayTicks--; 
    }
    else if (blackTicksLeft > 0)
    {
        blackTicksLeft--; 
    }
}

void Game::delayOrDecrementWhite()
{
    if (whiteDelayTicks > 0)
    {
        whiteDelayTicks--;
    }
    else if (whiteTicksLeft > 0)
    {
        whiteTicksLeft--;
    }
}

void Game::update() 
{ 
    delayOrDecrementCurrentPlayer();
    if(timerMode == TimerMode::Hourglass) { incrementOtherOneTick(); }
    if(blackTicksLeft == 0 || whiteTicksLeft == 0) 
    { 
        isGameOver = true; 
        updateHeader = true;
    }
}

void Game::resetBlackDelay() { blackDelayTicks = incrementSeconds; }
void Game::resetWhiteDelay() { whiteDelayTicks = incrementSeconds; }
void Game::resetCurrentPlayerDelay() { isBlackTurn ? resetBlackDelay() : resetWhiteDelay(); }

void Game::previousTimerMode()
{
    timerMode = (timerMode <= (TimerMode)0) ? (TimerMode)((int)TimerMode::TimerMode_MAX - 1) : (TimerMode)((int)timerMode - 1);
}

void Game::nextTimerMode()
{
    timerMode = (timerMode >= (TimerMode)((int)TimerMode::TimerMode_MAX - 1)) ? (TimerMode)0 : (TimerMode)((int)timerMode + 1);
}

bool Game::isBlinking()
{
    return isGameStarted && (isPaused || (!isGameOver && (isBlackInDanger() || isWhiteInDanger())));
}

void Game::decreaseMinutes()
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

void Game::increaseMinutes()
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


void Game::decreaseIncrement()
{
    incrementSeconds = (incrementSeconds > 0) ? incrementSeconds - 5 : 0;
}

void Game::increaseIncrement()
{
    incrementSeconds = (incrementSeconds < 60) ? incrementSeconds + 5 : 60;
}

void Game::selectNextMenuOption()
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

void Game::selectPreviousMenuOption()
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

void Game::commitMenuOption()
{
    switch (menuItem)
    {
    case MenuItem::Mode:
        menuItem = MenuItem::Minutes;
        break;

    case MenuItem::Minutes:
        if(timerMode == TimerMode::Fischer || timerMode == TimerMode::SimpleDelay)
        {
            menuItem = MenuItem::Increment;
        }
        else
        {
            incrementSeconds = 0;
            closeMenu();
        }
        break;

    case MenuItem::Increment:
        closeMenu();
        break;

    default:
        break;
    }
}

void Game::closeMenu()
{
    menuItem = MenuItem::Mode;
    isMenuOpen = false;
    blackTicksLeft = playtimeMinutes * 600l;
    whiteTicksLeft = playtimeMinutes * 600l;
    blackDelayTicks = timerMode == TimerMode::SimpleDelay ? incrementSeconds * 10 : 0;
    whiteDelayTicks = timerMode == TimerMode::SimpleDelay ? incrementSeconds * 10 : 0;
}

void Game::buttonPressed(Button button)
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
    else if(!isGameStarted){
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
    }
    else if(isPaused){
        switch (button)
        {
            case Button::Select:
                isPaused = false;
                break;

            default:
                break;
        }
    }
    else if(isGameOver)
    {
        switch (button)
        {
        case Button::Select:
            reset(timerMode, playtimeMinutes, incrementSeconds);
            break;

        default:
            break;
        }
    }
    else 
    {
        switch (button)
        {
        case Button::White:
            if (!isBlackTurn)
            {
                endWhiteTurn();
            }
            break;

        case Button::Black:
            if (isBlackTurn)
            {
                endBlackTurn();
            }
            break;

        case Button::Select:
            isPaused = true;
            break;

        default:
            break;
        }
    }
    updateHeader = true;
}