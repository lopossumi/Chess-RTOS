#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include "gameState.h"
#include "pinout.h"

void TaskReadButtons(void *pvParameters)
{
    pinMode(BUTTON_WHITE, INPUT);
    pinMode(BUTTON_BLACK, INPUT);
    pinMode(WHITE_LED_RED, OUTPUT);
    pinMode(WHITE_LED_GREEN, OUTPUT);
    pinMode(BUTTON_SELECT, OUTPUT);

    for(;;)
    {
        bool whiteButtonPressed = false;
        bool blackButtonPressed = false;
        bool selectButtonPressed = false;

        while(!whiteButtonPressed 
            && !blackButtonPressed
            && !selectButtonPressed)
        {
            whiteButtonPressed = digitalRead(BUTTON_WHITE);
            blackButtonPressed = digitalRead(BUTTON_BLACK);
            selectButtonPressed = digitalRead(BUTTON_SELECT);
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        auto button = whiteButtonPressed
            ? Button::White
            : blacButtonPressed
            ? Button::Black
            : Button::Select;
        if(whiteButtonPressed && blackButtonPressed)
            
        Game *gameState = static_cast<Game *>(pvParameters);
        gameState->buttonPressed(button);

        // TODO: Replace with led task
        if(gameState->isGameStarted && gameState->isBlackTurn)
        {
            digitalWrite(WHITE_LED_RED, LED_ON);
            digitalWrite(WHITE_LED_GREEN, LED_OFF);
        }
        else
        {
            digitalWrite(WHITE_LED_RED, LED_OFF);
            digitalWrite(WHITE_LED_GREEN, LED_ON);
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
