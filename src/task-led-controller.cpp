#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include "game-state.h"
#include "pinout.h"

void TaskLedController(void *pvParameters)
{
    pinMode(WHITE_LED_RED, OUTPUT);
    pinMode(WHITE_LED_GREEN, OUTPUT);
    pinMode(BLACK_LED_RED, OUTPUT);
    pinMode(BLACK_LED_GREEN, OUTPUT);
    
    for(;;)
    {
        auto *gameState = static_cast<Game *>(pvParameters);
        
        if(gameState->isGameOver)
        {
            digitalWrite(gameState->isBlackTurn ? BLACK_LED_RED : WHITE_LED_RED, LED_ON);
            digitalWrite(gameState->isBlackTurn ? WHITE_LED_RED : BLACK_LED_RED, LED_OFF);
            digitalWrite(WHITE_LED_GREEN, LED_OFF);
            digitalWrite(BLACK_LED_GREEN, LED_OFF);
        }
        else if(gameState->isGameStarted)
        {
            digitalWrite(gameState->isBlackTurn ? BLACK_LED_GREEN : WHITE_LED_GREEN, LED_ON);
            digitalWrite(gameState->isBlackTurn ? WHITE_LED_GREEN : BLACK_LED_GREEN, LED_OFF);
            digitalWrite(WHITE_LED_RED, LED_OFF);
            digitalWrite(BLACK_LED_RED, LED_OFF);
        }
        else
        {
            digitalWrite(WHITE_LED_RED, LED_OFF);
            digitalWrite(WHITE_LED_GREEN, LED_OFF);
            digitalWrite(BLACK_LED_RED, LED_OFF);
            digitalWrite(BLACK_LED_GREEN, LED_OFF);
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
