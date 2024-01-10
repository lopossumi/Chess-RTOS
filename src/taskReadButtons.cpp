#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include "gameState.h"
#include "pinout.h"

void TaskReadButtons(void *pvParameters)
{
    pinMode(BUTTON_WHITE, INPUT);
    pinMode(BUTTON_BLACK, INPUT);
    for(;;)
    {
        vTaskDelay(pdMS_TO_TICKS(200));
        digitalWrite(WHITE_LED_RED, LED_OFF);
        digitalWrite(WHITE_LED_GREEN, LED_OFF);

        bool whiteButtonPressed = false;
        bool blackButtonPressed = false;

        while(!whiteButtonPressed && !blackButtonPressed)
        {
            whiteButtonPressed = digitalRead(BUTTON_WHITE);
            blackButtonPressed = digitalRead(BUTTON_BLACK);
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }
        digitalWrite(WHITE_LED_RED, whiteButtonPressed ? LED_ON : LED_OFF);
        digitalWrite(WHITE_LED_GREEN, blackButtonPressed ? LED_ON : LED_OFF);

        auto button = whiteButtonPressed ? Button::White : Button::Black;
        if(whiteButtonPressed && blackButtonPressed)
        {
            button = Button::Select;
        }
        GameState *gameState = static_cast<GameState *>(pvParameters);
        gameState->buttonPressed(button);
    }
}
