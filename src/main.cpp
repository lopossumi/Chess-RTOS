#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <Wire.h>

#include "enums.hpp"
#include "player.hpp"
#include "taskUpdateScreen.hpp"
#include "gameState.hpp"
#include "main.hpp"

void TaskUpdateScreen(void *pvParameters);
void TaskReadButton(void *pvParameters);
void TaskGameLoop(void *pvParameters);

TimerMode timerMode;

int playtimeMinutes;
int incrementSeconds;
bool blink;

GameState gameState = GameState();

void setup()
{
    timerMode = (TimerMode)EEPROM.read(0);
    playtimeMinutes = EEPROM.read(1);
    incrementSeconds = EEPROM.read(2);

    // FreeRTOS  Function Name      Task Name       Stack   Params  Prio  Handle
    xTaskCreate( TaskUpdateScreen,  "UpdateScreen", 256,    (void*)&gameState,   2,    NULL ); // Pass the address of gameState
    xTaskCreate( TaskReadButton,    "ReadButton",   128,    NULL,   1,    NULL );
    xTaskCreate( TaskGameLoop,      "GameLoop",     128,    NULL,   3,    NULL );

    vTaskStartScheduler();
}

void loop() {}

Button convertButtonState(int value)
{
    switch (value)
    {
        case 1000 ... 1023: return Button::None;
        case 500 ... 699:   return Button::Select;
        case 300 ... 499:   return Button::Left;
        case 0 ... 49:      return Button::Right;
        default:            return Button::None;
    }
}

void TaskGameLoop(void *pvParameters __attribute__((unused)))
{
    for(;;)
    {
        switch (gameState.getCurrentState())
        {
            case ClockState::Welcome:
                vTaskDelay(pdMS_TO_TICKS(3000));
                currentState = ClockState::ModeSet;
                break;
            
            case ClockState::MinuteSet:
                switch(timerMode)
                {
                    case TimerMode::SuddenDeath:
                    case TimerMode::Hourglass:
                        incrementSeconds = 0;
                        break;

                    case TimerMode::Fischer:
                        break;

                    default:
                        break;
                }
                break;
            
            case ClockState::Ready:
                break;

            case ClockState::Play:
                switch(timerMode)
                {
                    case TimerMode::SimpleDelay:
                        gameState.delayOrDecrementCurrentPlayer();
                        break;

                    case TimerMode::Hourglass:
                        gameState.incrementOtherOneTick();
                        gameState.decrementCurrentPlayer();
                        break;

                    default:
                        gameState.decrementCurrentPlayer();
                        break;
                }
                if(gameState.isBlackOutOfTime() || gameState.isWhiteOutOfTime())
                {
                    currentState = ClockState::GameOver;
                    blink = false;
                }
                break;
            
            case ClockState::Pause:
                blink = true;
                break;
            default:
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void TaskReadButton(void *pvParameters __attribute__((unused)))
{
    for (;;)
    {
        // Read button value. Debounce by averaging 2 readings.
        int reading = analogRead(0);
        int value = reading;
        while (reading < 1000)
        {
            value += reading;
            value /= 2;
            reading = analogRead(0);
            vTaskDelay(1);
        }
        auto button = convertButtonState(value);
        if(button == Button::None) continue;

        switch (currentState)
        {
            // Welcome screen:
            // * Buttons do nothing
            case ClockState::Welcome:
                break;

            // Mode select screen:
            // * Select button continues to next screen
            // * Left button cycles through modes
            // * Right button cycles through modes
            case ClockState::ModeSet:
                switch (button)
                {
                    case Button::Select:
                        currentState = ClockState::MinuteSet;
                        break;

                    case Button::Left:
                        timerMode = (timerMode <= (TimerMode)0) 
                            ? (TimerMode)((int)TimerMode::TimerMode_MAX - 1) 
                            : (TimerMode)((int)timerMode - 1);
                        break;

                    case Button::Right:
                        timerMode = (timerMode >= (TimerMode)((int)TimerMode::TimerMode_MAX - 1)) 
                            ? (TimerMode)0 
                            : (TimerMode)((int)timerMode + 1);
                        break;
                
                    default:
                        break;
                }
                break;
            
            // Minute set screen:
            // * Select button continues to next screen
            // * Left button decrements minutes
            // * Right button increments minutes
            // * Minute options are 1, 2, 3, 4, 5, 10, 15, 20, 25, 30, 45, 60, 90
            case ClockState::MinuteSet:
                switch (button)
                {
                    case Button::Select:
                        switch (timerMode)
                        {
                            case TimerMode::SuddenDeath:
                            case TimerMode::Hourglass:
                                currentState = ClockState::Ready;
                                break;

                            default:
                                currentState = ClockState::SecondSet;
                                break;
                        }
                        break;

                    case Button::Left:
                        switch (playtimeMinutes)
                        {
                            case 61 ... 90: playtimeMinutes = 60;   break;
                            case 31 ... 60: playtimeMinutes -= 15;  break;
                            case 6  ... 30: playtimeMinutes -= 5;   break;
                            case 2  ...  5: playtimeMinutes -= 1;   break;
                            default:
                                break;
                        }
                        break;

                    case Button::Right:
                        switch(playtimeMinutes)
                        {
                            case 1  ...  4: playtimeMinutes += 1;   break;
                            case 5  ... 29: playtimeMinutes += 5;   break;
                            case 30 ... 59: playtimeMinutes += 15;  break;
                            case 60 ... 90: playtimeMinutes = 90;   break;
                            default:
                                break;
                        }
                        break;
                
                    default:
                        break;
                }
                break;

            // Second set screen:
            // * Select button sets both players minutes to playtimeMinutes and continues to next screen
            // * Left button decrements seconds 5 at a time
            // * Right button increments seconds 5 at a time
            case ClockState::SecondSet:
                switch (button)
                {
                    case Button::Select:
                        currentState = ClockState::Ready;
                        break;

                    case Button::Left:
                        incrementSeconds = (incrementSeconds > 0) ? incrementSeconds - 5 : 0;
                        break;

                    case Button::Right:
                        incrementSeconds = (incrementSeconds < 60) ? incrementSeconds + 5 : 60;
                        break;
                
                    default:
                        break;
                }
                break;

            // Ready screen:
            // * Select button does nothing
            // * Left button starts the game
            // * Right button starts the game
            case ClockState::Ready:
                switch (button)
                {
                    case Button::Select:
                        EEPROM.write(0, (int)timerMode);
                        EEPROM.write(1, playtimeMinutes);
                        EEPROM.write(2, incrementSeconds);
                        break;
                    case Button::Left:
                        gameState.setCurrentPlayerToWhite();
                        currentState = ClockState::Play;                        
                        break;

                    case Button::Right:
                        gameState.setCurrentPlayerToBlack();
                        currentState = ClockState::Play;
                        break;
                
                    default:
                        break;
                }
                break;

            // Play screen:
            // * Select button pauses the game
            // * Left button is black player button
            // * Right button is white player button
            case ClockState::Play:
                switch (button)
                {
                    case Button::Select:
                        blink = true;
                        currentState = ClockState::Pause;
                        break;
                    
                    case Button::Left:
                    case Button::Right:
                        if (button == Button::Left && gameState.isWhiteTurnNow()) break;
                        if (button == Button::Right && gameState.isBlackTurnNow()) break;

                        switch (timerMode)
                        {
                            case TimerMode::SimpleDelay:
                                gameState.resetCurrentPlayerDelay();
                                break;
                            case TimerMode::Fischer:
                                gameState.incrementCurrentPlayer();
                                break;
                            default:
                                break;
                        }
                        gameState.toggleCurrentPlayer();
                        
                        break;
                    
                    default:
                        break;               
                }
                break;

            // Pause screen:
            // * Select button resumes the game
            // * Left button does nothing
            // * Right button does nothing
            case ClockState::Pause:
                switch (button)
                {
                    case Button::Select:
                        blink = false;
                        currentState = ClockState::Play;
                        break;
                
                    default:
                        break;
                }
                break;

            // Game over screen:
            // * Select button returns to set minutes screen
            // * Left button does nothing
            // * Right button does nothing
            case ClockState::GameOver:
                switch (button)
                {
                    case Button::Select:
                        currentState = ClockState::ModeSet;
                        break;
                
                    default:
                        break;
                }
                break;
            
            default:
                break;
        }
    }
}
