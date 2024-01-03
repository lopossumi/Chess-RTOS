#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Wire.h>

#include "main.h"
#include "player.cpp"

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void TaskUpdateScreen(void *pvParameters);
void TaskReadButton(void *pvParameters);
void TaskGameLoop(void *pvParameters);

ClockState currentState;

TimerMode timerMode;
int buttonValue;
int setMinutes = 15;
int setSeconds = 30;
bool blink = false;

Player black;
Player white;

void setup()
{
    lcd.begin(16, 2);
    currentState = ClockState::Welcome;
    timerMode = TimerMode::SuddenDeath;

    xTaskCreate(
        TaskUpdateScreen, 
        "DigitalRead",
        128,
        NULL,
        2,
        NULL);

    xTaskCreate(
        TaskReadButton,
        "AnalogRead",
        128,
        NULL,
        1,
        NULL);
    
    xTaskCreate(
        TaskGameLoop,
        "GameLoop",
        128,
        NULL,
        3,
        NULL);
}

void loop()
{
}

void printSetpoints()
{
    // set the cursor to column 0, line 1
    // print setpoints for minutes and seconds
    // Examples: (underscore is blank space)
    // ___15m_+_30s___     
    // ____5m_+_0s____      
    // ___90m_+_0s____
    // First create an interpolated string for the row
    // Then print the string

    lcd.setCursor(0, 1);
    char buffer[17];
    sprintf(buffer, "    %02dm + %02ds   ", setMinutes, setSeconds);
    lcd.print(buffer);
}

void printTimes()
{
    // set the cursor to column 0, line 1
    // print times for black and white players
    // time is in the format mm:ss.t
    // Example:
    // 15:30.0  15:30.0
    // First create an interpolated string for the row
    // Then print the string
    
    lcd.setCursor(0, 1);
    char buffer[17];
    sprintf(buffer, "%02d:%02d.%d  %02d:%02d.%d", 
        black.getMinutes(), black.getSeconds(), black.getTenths(), 
        white.getMinutes(), white.getSeconds(), white.getTenths());
    lcd.print(buffer);
}

void TaskUpdateScreen(void *pvParameters __attribute__((unused))) // This is a Task.
{
    for (;;) // A Task shall never return or exit.
    {
        if(blink)
        {
            lcd.clear();
            vTaskDelay(10);
        }
        // Print header:
        lcd.setCursor(0, 0);
        switch (currentState)
        {
        case ClockState::Welcome:
            lcd.print("  Chess Clock   ");
            lcd.setCursor(0, 1);
            lcd.print(" (c) milo 2024  ");
            break;

        case ClockState::ModeSet:
            lcd.print("Select mode:    ");
            lcd.setCursor(0, 1);
            switch (timerMode)
            {
                case TimerMode::SuddenDeath:    lcd.print("< Sudden Death >");  break;
                case TimerMode::Fischer:        lcd.print("< Fischer      >");  break;
                case TimerMode::Hourglass:      lcd.print("< Hourglass    >");  break;
                case TimerMode::ByoYomi:        lcd.print("< Byo-Yomi     >");  break;

                default:
                    break;
            }
            break;
        case ClockState::MinuteSet:
            lcd.print("Set minutes:    ");
            printSetpoints();
            break;
        case ClockState::SecondSet:
            lcd.print("Set increment:  ");
            printSetpoints();
            break;
        case ClockState::Ready:
            lcd.print(" Ready to Start ");
            printTimes();
            break;
        case ClockState::White:
            lcd.print("         White >");
            printTimes();

            break;
        case ClockState::Black:
            lcd.print("< Black         ");
            printTimes();

            break;
        case ClockState::PausedWithBlack:
            lcd.print("< Black         ");
            printTimes();
            break;
        case ClockState::PausedWithWhite:
            lcd.print("         White >");
            printTimes();
            break;
        case ClockState::GameOverWhiteWins:
            lcd.print("   Game Over    ");
            lcd.setCursor(0, 1);
            lcd.print("   White wins!  ");
            break;

        case ClockState::GameOverBlackWins:
            lcd.print("   Game Over    ");
            lcd.setCursor(0, 1);
            lcd.print("   Black wins!  ");
            break;

        default:
            break;
        }
        lcd.print(buttonValue);

        vTaskDelay(blink ? 50 : 1);
    }
}

ButtonState convertButtonState(int buttonValue)
{
    if (buttonValue >= 1000)            return ButtonState::None;
    else if (buttonValue < 50)          return ButtonState::Right;
    else if (buttonValue < 150)         return ButtonState::Up;
    else if (buttonValue < 300)         return ButtonState::Down;
    else if (buttonValue < 500)         return ButtonState::Left;
    else if (buttonValue < 700)         return ButtonState::Select;
    else                                return ButtonState::None;   
}

void TaskGameLoop(void *pvParameters __attribute__((unused)))
{
    for(;;)
    {
        switch (currentState)
        {
            case ClockState::Welcome:
                vTaskDelay(pdMS_TO_TICKS(2000));
                currentState = ClockState::ModeSet;
                break;

            case ClockState::White:
                white.decrement();
                if(white.isOutOfTime())
                {
                    currentState = ClockState::GameOverBlackWins;
                }
                break;

            case ClockState::Black:
                black.decrement();
                if(black.isOutOfTime())
                {
                    currentState = ClockState::GameOverWhiteWins;
                }
                break;
            
            default:
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void TaskReadButton(void *pvParameters __attribute__((unused)))
{
    for (;;)
    {
        int temp = analogRead(0);
        int value = temp;
        while (temp < 1000)
        {
            value += temp;
            value /= 2;
            temp = analogRead(0);
            vTaskDelay(1);
        }

        auto button = convertButtonState(value);
        switch (currentState)
        {
            // Welcome screen:
            // * Buttons do nothing
            case ClockState::Welcome:
                break;

            // Mode select screen:
            // * Select button continues to next screen
            // * Left button cycles through modes
            // * Right button cycles through modes
            case ClockState::ModeSet:
                switch (button)
                {
                    case ButtonState::Select:
                        currentState = ClockState::MinuteSet;
                        break;

                    case ButtonState::Left:
                        timerMode = (timerMode == (TimerMode)0) 
                            ? (TimerMode)((int)TimerMode::TimerMode_MAX - 1) 
                            : (TimerMode)((int)timerMode - 1);
                        break;

                    case ButtonState::Right:
                        timerMode = (timerMode == (TimerMode)((int)TimerMode::TimerMode_MAX - 1)) 
                            ? (TimerMode)0 
                            : (TimerMode)((int)timerMode + 1);
                        break;
                
                    default:
                        break;
                }
                break;
            
            // Minute set screen:
            // * Select button continues to next screen
            // * Left button decrements minutes
            // * Right button increments minutes
            // * Minute options are 1, 2, 3, 5, 10, 15, 20, 25, 30, 45, 60, 90
            case ClockState::MinuteSet:
                switch (button)
                {
                    case ButtonState::Select:
                        currentState = ClockState::SecondSet;
                        break;

                    case ButtonState::Left:
                        if(setMinutes > 60) setMinutes = 60;
                        else if(setMinutes > 15) setMinutes -= 15;
                        else if(setMinutes > 5) setMinutes -= 5;
                        else if(setMinutes > 1) setMinutes -= 1;
                        break;

                    case ButtonState::Right:
                        if(setMinutes < 5) setMinutes += 1;
                        else if(setMinutes < 15) setMinutes += 5;
                        else if(setMinutes < 60) setMinutes += 15;
                        else if(setMinutes < 90 ) setMinutes = 90;
                        break;
                
                    default:
                        break;
                }
                break;

            // Second set screen:
            // * Select button sets both players minutes to setMinutes and continues to next screen
            // * Left button decrements seconds 5 at a time
            // * Right button increments seconds 5 at a time
            case ClockState::SecondSet:
                switch (button)
                {
                    case ButtonState::Select:
                        white.setMinutes(setMinutes);
                        black.setMinutes(setMinutes);
                        blink = true;
                        currentState = ClockState::Ready;
                        break;

                    case ButtonState::Left:
                        setSeconds = (setSeconds > 0) ? setSeconds - 5 : 0;
                        break;

                    case ButtonState::Right:
                        setSeconds = (setSeconds < 60) ? setSeconds + 5 : 60;
                        break;
                
                    default:
                        break;
                }
                break;

            // Ready screen:
            // * Select button does nothing
            // * Left button starts the game
            // * Right button starts the game
            case ClockState::Ready:
                switch (button)
                {
                    case ButtonState::Left:
                        blink = false;
                        currentState = ClockState::White;
                        break;

                    case ButtonState::Right:
                        blink = false;
                        currentState = ClockState::Black;
                        break;
                
                    default:
                        break;
                }
                break;

            // White to play screen:
            // * Select button pauses the game
            // * Left button does nothing
            // * Right button sets the game to black to play
            case ClockState::White:
                switch (button)
                {
                    case ButtonState::Select:
                        blink = true;
                        currentState = ClockState::PausedWithWhite;
                        break;

                    case ButtonState::Right:
                        currentState = ClockState::Black;
                        break;
                
                    default:
                        break;
                }
                break;
            
            // Black to play screen:
            // * Select button pauses the game
            // * Left button sets the game to white to play
            // * Right button does nothing
            case ClockState::Black:
                switch (button)
                {
                    case ButtonState::Select:
                        blink = true;
                        currentState = ClockState::PausedWithBlack;
                        break;

                    case ButtonState::Left:
                        currentState = ClockState::White;
                        break;
                
                    default:
                        break;
                }
                break;

            // Paused with white to play screen:
            // * Select button resumes the game
            // * Left button does nothing
            // * Right button does nothing
            case ClockState::PausedWithWhite:
                switch (button)
                {
                    case ButtonState::Select:
                        blink = false;
                        currentState = ClockState::White;
                        break;
                
                    default:
                        break;
                }
                break;

            // Paused with black to play screen:
            // * Select button resumes the game
            // * Left button does nothing
            // * Right button does nothing
            case ClockState::PausedWithBlack:
                switch (button)
                {
                    case ButtonState::Select:
                        blink = false;
                        currentState = ClockState::Black;
                        break;
                
                    default:
                        break;
                }
                break;

            // Both Game over screens:
            // * Select button returns to set minutes screen
            // * Left button does nothing
            // * Right button does nothing
            case ClockState::GameOverBlackWins:
            case ClockState::GameOverWhiteWins:
                switch (button)
                {
                    case ButtonState::Select:
                        currentState = ClockState::MinuteSet;
                        break;
                
                    default:
                        break;
                }
                break;
            
            default:
                break;
        }
    }
}
