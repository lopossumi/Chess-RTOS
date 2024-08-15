#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include "game-state.h"
#include "pinout.h"
#include "task-io-controller.h"
int leds = B1111;

#define C4 262
#define A4 440
#define B4 494
#define C5 523
#define D5 587
#define Eb5 622
#define E5 659
#define F5 698
#define G5 784
#define A5 880
#define Bb5 932
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
    pinMode(ENC_CLK, INPUT_PULLUP);
    pinMode(ENC_DT, INPUT_PULLUP);

    uint8_t input = 0;
    uint8_t lastEncoderValue = 0;

    turnLeds(B0000);

    playNote(C4, 10);
    vTaskDelay(pdMS_TO_TICKS(500));
    playNote(C4, 10);
    vTaskDelay(pdMS_TO_TICKS(300));
    playNote(C4, 10);
    vTaskDelay(pdMS_TO_TICKS(200));
    playNote(C4, 10);
    vTaskDelay(pdMS_TO_TICKS(100));
    playNote(C4, 10);
    vTaskDelay(pdMS_TO_TICKS(50));
    playNote(C4, 10);
    vTaskDelay(pdMS_TO_TICKS(40));
    playNote(C4, 10);
    vTaskDelay(pdMS_TO_TICKS(40));
    playNote(C4, 10);
    vTaskDelay(pdMS_TO_TICKS(40));
    playNote(C4, 10);
    vTaskDelay(pdMS_TO_TICKS(40));
    playNote(C4, 10);
    vTaskDelay(pdMS_TO_TICKS(40));

    playHappyTone();
    playHappyTone();

    auto *gameState = static_cast<Game *>(pvParameters);

    for(;;)
    {
        turnLeds(leds);
        input |= digitalRead(BUTTON_WHITE);
        input |= digitalRead(BUTTON_BLACK) << 1;
        input |= digitalRead(ENC_SW) << 2;
        input |= digitalRead(ENC_DT) << 3;
        input |= digitalRead(ENC_CLK) << 4;


        auto button = input & 0b00001 ? Button::White
                    : input & 0b00010 ? Button::Black
                    : input & 0b00100 ? Button::Select
                    : Button::Button_MAX;

        uint8_t encoderValue = input >> 3;
        if(encoderValue != lastEncoderValue && gameState->isMenuOpen)
        {
            // if encoder was turned left, play a lower tone
            bool left = (lastEncoderValue == 0b00 && encoderValue == 0b11)
                     || (lastEncoderValue == 0b11 && encoderValue == 0b01)
                     || (lastEncoderValue == 0b01 && encoderValue == 0b00);
            tone(BUZZER, left ? C5 : C6, 20);
            lastEncoderValue = encoderValue;
            button = left ? Button::Left : Button::Right;
        }
        input = 0;

        bool playSound = gameState->buttonPressed(button);

        bool isBlackTurn = gameState->isBlackTurn;
        
        if(gameState->isGameOver)           { turnLeds(isBlackTurn ? B1001 : B0110); }
        else if(gameState->isGameStarted)   { turnLeds(isBlackTurn ? B0100 : B1000); }
        else if(!gameState->isMenuOpen)     { turnLeds(B1100); }

        if(button == Button::White
            && playSound)
        {
            playHappyTone();
            vTaskDelay(pdMS_TO_TICKS(200));
        }
        else if(button == Button::Black
            && playSound)
        {
            playSadTone();
            vTaskDelay(pdMS_TO_TICKS(200));
        }
        else if(button == Button::Select
            && playSound)
        {
            playNote(G5, 20);
            vTaskDelay(pdMS_TO_TICKS(200));
        }
        vTaskDelay(pdMS_TO_TICKS(30));
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

    if(value & B0010 || value & B0011)
    {
        playSadTone();
        playSadTone();
        playSadTone();
        playSadTone();
        playSadTone();
    }
    
    if(leds == B1100)
    {
        playHappyTone();
    }

    digitalWrite(WHITE_LED_GREEN, (value & B1000) ? LED_ON : LED_OFF);
    digitalWrite(BLACK_LED_GREEN, (value & B0100) ? LED_ON : LED_OFF);
    digitalWrite(WHITE_LED_RED, (value & B0010) ? LED_ON : LED_OFF);
    digitalWrite(BLACK_LED_RED, (value & B0001) ? LED_ON : LED_OFF);
    leds = value;
}

void playNote(int freq, int duration)
{
    tone(BUZZER, freq, duration);
    vTaskDelay(pdMS_TO_TICKS(duration));
}

void playHappyTone()
{
    playNote(C5, 40);
    playNote(E5, 40);
    playNote(G5, 40);
    playNote(B5, 40);
    playNote(C6, 40);
}

void playSadTone()
{
    playNote(C5, 40);
    playNote(Bb5,40);
    playNote(G5, 40);
    playNote(Eb5,40);
    playNote(C5, 40);
}