#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include "game-state.h"
#include "pinout.h"
#include "task-io-controller.h"
int leds = B1111;

#define A4 440
#define B4 494
#define C5 523
#define D5 587
#define E5 659
#define F5 698
#define G5 784
#define A5 880
#define B5 988
#define C6 1047
#define D6 1175
#define E6 1319

void TaskIOController(void *pvParameters)
{
    pinMode(BUTTON_WHITE, INPUT);
    pinMode(BUTTON_BLACK, INPUT);
    pinMode(BUTTON_SELECT, OUTPUT);
    pinMode(WHITE_LED_RED, OUTPUT);
    pinMode(WHITE_LED_GREEN, OUTPUT);
    pinMode(BLACK_LED_RED, OUTPUT);
    pinMode(BLACK_LED_GREEN, OUTPUT);
    pinMode(BUZZER, OUTPUT);

    bool whiteButtonPressed = false;
    bool blackButtonPressed = false;
    bool selectButtonPressed = false;

    turnLeds(B0000);

    tone(BUZZER, G5, 320);
    vTaskDelay(pdMS_TO_TICKS(320));
    tone(BUZZER, A5, 320);
    vTaskDelay(pdMS_TO_TICKS(320));
    tone(BUZZER, G5, 320);
    vTaskDelay(pdMS_TO_TICKS(320));
    tone(BUZZER, C6, 320);
    vTaskDelay(pdMS_TO_TICKS(320));
    tone(BUZZER, G5, 320);
    vTaskDelay(pdMS_TO_TICKS(320));
    tone(BUZZER, A5, 320);
    vTaskDelay(pdMS_TO_TICKS(320));
    tone(BUZZER, G5, 320);
    vTaskDelay(pdMS_TO_TICKS(320));
    vTaskDelay(pdMS_TO_TICKS(320));
    tone(BUZZER, A5, 320);
    vTaskDelay(pdMS_TO_TICKS(320));
    tone(BUZZER, A5, 320);
    vTaskDelay(pdMS_TO_TICKS(320));
    tone(BUZZER, G5, 320);
    vTaskDelay(pdMS_TO_TICKS(320));
    tone(BUZZER, A5, 320);
    vTaskDelay(pdMS_TO_TICKS(320));
    tone(BUZZER, G5, 160);
    vTaskDelay(pdMS_TO_TICKS(160));
    tone(BUZZER, F5, 160);
    vTaskDelay(pdMS_TO_TICKS(160));
    tone(BUZZER, E5, 160);
    vTaskDelay(pdMS_TO_TICKS(160));
    tone(BUZZER, D5, 160);
    vTaskDelay(pdMS_TO_TICKS(160));
    tone(BUZZER, E5, 320);
    vTaskDelay(pdMS_TO_TICKS(320));
    tone(BUZZER, C5, 320);
    vTaskDelay(pdMS_TO_TICKS(320));

    for(;;)
    {
        turnLeds(leds);
        whiteButtonPressed = digitalRead(BUTTON_WHITE);
        blackButtonPressed = digitalRead(BUTTON_BLACK);
        selectButtonPressed = digitalRead(BUTTON_SELECT);
        
        auto button = whiteButtonPressed ? Button::White
                    : blackButtonPressed ? Button::Black
                    : selectButtonPressed ? Button::Select
                    : Button::Button_MAX;
                
        auto *gameState = static_cast<Game *>(pvParameters);
        bool playSound = gameState->buttonPressed(button);

        bool isBlackTurn = gameState->isBlackTurn;
        
        if(gameState->isGameOver)           { turnLeds(isBlackTurn ? B1001 : B0110); }
        else if(gameState->isGameStarted)   { turnLeds(isBlackTurn ? B0100 : B1000); }
        else if(!gameState->isMenuOpen)     { turnLeds(B1100); }

        if(playSound)
        {
            tone(BUZZER, 880, 50);
            vTaskDelay(pdMS_TO_TICKS(200));
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

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
    if(leds == value)
    {
        return;
    }

    digitalWrite(WHITE_LED_GREEN, (value & B1000) ? LED_ON : LED_OFF);
    digitalWrite(BLACK_LED_GREEN, (value & B0100) ? LED_ON : LED_OFF);
    digitalWrite(WHITE_LED_RED, (value & B0010) ? LED_ON : LED_OFF);
    digitalWrite(BLACK_LED_RED, (value & B0001) ? LED_ON : LED_OFF);
    
    leds = value;
}