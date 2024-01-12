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
    pinMode(ENC_SW, OUTPUT);
    pinMode(WHITE_LED_RED, OUTPUT);
    pinMode(WHITE_LED_GREEN, OUTPUT);
    pinMode(BLACK_LED_RED, OUTPUT);
    pinMode(BLACK_LED_GREEN, OUTPUT);
    pinMode(BUZZER, OUTPUT);
    pinMode(A1, INPUT_PULLUP);

    pinMode(ENC_CLK, INPUT_PULLUP);
    pinMode(ENC_DT, INPUT_PULLUP);

    bool whiteButtonPressed = false;
    bool blackButtonPressed = false;
    bool selectButtonPressed = false;
    bool dt = false;
    bool clk = false;
    uint8_t lastEncoderValue = 0;

    turnLeds(B0000);

    tone(BUZZER, C5, 80);
    vTaskDelay(pdMS_TO_TICKS(80));
    tone(BUZZER, E5, 80);
    vTaskDelay(pdMS_TO_TICKS(80));
    tone(BUZZER, G5, 80);
    vTaskDelay(pdMS_TO_TICKS(80));
    tone(BUZZER, B5, 80);
    vTaskDelay(pdMS_TO_TICKS(80));
    tone(BUZZER, C6, 80);
    vTaskDelay(pdMS_TO_TICKS(80));

    for(;;)
    {
        turnLeds(leds);
        whiteButtonPressed = digitalRead(BUTTON_WHITE);
        blackButtonPressed = digitalRead(BUTTON_BLACK);
        selectButtonPressed = digitalRead(ENC_SW);
        dt = digitalRead(ENC_DT);
        clk = digitalRead(ENC_CLK);

        uint8_t encoderValue = (dt << 1) | clk;
        if(encoderValue != lastEncoderValue)
        {
            // if encoder was turned left, play a lower tone
            bool left = (lastEncoderValue == 0b00 && encoderValue == 0b11)
                     || (lastEncoderValue == 0b11 && encoderValue == 0b10)
                     || (lastEncoderValue == 0b10 && encoderValue == 0b00);
            tone(BUZZER, left ? 440 : 880, 20);
            vTaskDelay(pdMS_TO_TICKS(20));
            lastEncoderValue = encoderValue;
        }
        turnLeds(encoderValue);

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
            tone(BUZZER, 880, 20);
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