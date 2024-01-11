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

    for(;;)
    {
        bool whiteButtonPressed = false;
        bool blackButtonPressed = false;

        while(!whiteButtonPressed && !blackButtonPressed)
        {
            whiteButtonPressed = digitalRead(BUTTON_WHITE);
            blackButtonPressed = digitalRead(BUTTON_BLACK);
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        auto button = whiteButtonPressed ? Button::White : Button::Black;
        if(whiteButtonPressed && blackButtonPressed)
        {
            button = Button::Select;
        }
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
