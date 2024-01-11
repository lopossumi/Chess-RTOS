#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include "game-state.h"
#include "pinout.h"

/// @brief Turn on/off the leds 4 at a time.
/// B0000 = all off,
/// B1111 = all on,
/// B1000 = white green on,
/// B0100 = black green on,
/// B0010 = white red on,
/// B0001 = black red on.
/// @param value 4 bit value.
void turnLeds(uint8_t value)
{
    digitalWrite(WHITE_LED_GREEN, (value & B1000) ? LED_ON : LED_OFF);
    digitalWrite(BLACK_LED_GREEN, (value & B0100) ? LED_ON : LED_OFF);
    digitalWrite(WHITE_LED_RED, (value & B0010) ? LED_ON : LED_OFF);
    digitalWrite(BLACK_LED_RED, (value & B0001) ? LED_ON : LED_OFF);
}

void TaskLedController(void *pvParameters)
{
    pinMode(WHITE_LED_RED, OUTPUT);
    pinMode(WHITE_LED_GREEN, OUTPUT);
    pinMode(BLACK_LED_RED, OUTPUT);
    pinMode(BLACK_LED_GREEN, OUTPUT);
    
    turnLeds(B0000);
    for(;;)
    {
        auto *gameState = static_cast<Game *>(pvParameters);
        bool isBlackTurn = gameState->isBlackTurn;
        if(gameState->isGameOver)
        {
            turnLeds(isBlackTurn ? B1001 : B0110);
        }
        else if(gameState->isGameStarted)
        {
            if(gameState->isBlinking())
            {
                turnLeds(B0000);
                vTaskDelay(pdMS_TO_TICKS(200));
            }
            turnLeds(isBlackTurn ? B0100 : B1000);
        }
        else if(!gameState->isMenuOpen)
        {
            turnLeds(B1100);
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
