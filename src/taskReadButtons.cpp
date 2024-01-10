#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include "gameState.h"

#define BUTTON_WHITE 2
#define BUTTON_BLACK 6
#define WHITE_LED_GREEN 3
#define WHITE_LED_RED 4

#define LED_ON LOW
#define LED_OFF HIGH

void TaskReadButtons(void *pvParameters)
{
    pinMode(BUTTON_WHITE, INPUT);
    pinMode(BUTTON_BLACK, INPUT);
    pinMode(WHITE_LED_RED, OUTPUT);
    pinMode(WHITE_LED_GREEN, OUTPUT);    

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
        GameState *gameState = static_cast<GameState *>(pvParameters);

        if(gameState->isMenuOpen)
        {
            switch (gameState->menuItem)
            {
                case MenuItem::Mode:
                    switch(button)
                    {
                        case Button::White:
                            gameState->nextTimerMode();
                            break;
                        case Button::Black:
                            gameState->previousTimerMode();
                            break;
                        default:
                            gameState->setTimerMode();
                            break;
                    }
                    break;
                default:
                    break;
            
            }
        }
    }
}
