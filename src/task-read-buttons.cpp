#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include "game-state.h"
#include "pinout.h"

void TaskReadButtons(void *pvParameters)
{
    pinMode(BUTTON_WHITE, INPUT);
    pinMode(BUTTON_BLACK, INPUT);
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
            vTaskDelay(pdMS_TO_TICKS(20));
            continue;
        }

        auto button = whiteButtonPressed ? Button::White
                    : blackButtonPressed ? Button::Black
                    : Button::Select;
            
        auto *gameState = static_cast<Game *>(pvParameters);
        bool playSound = gameState->buttonPressed(button);

        vTaskDelay(pdMS_TO_TICKS(300));
    }
}
