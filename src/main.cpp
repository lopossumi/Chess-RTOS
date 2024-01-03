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
int playtimeMinutes = 15;
int incrementSeconds = 30;
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

/** 
 * Print setpoints for minutes and seconds on the second row of the LCD.
 * Time is in the format mm:ss
 * ### Examples ###
 * ```
 *     15m + 30s   
 *     05m + 00s   
 *     90m + 00s   
 * ```
 */
void printSetpoints()
{
    lcd.setCursor(0, 1);
    char buffer[17];
    sprintf(buffer, "    %02dm + %02ds   ", playtimeMinutes, incrementSeconds);
    lcd.print(buffer);
}

/** 
 * Print times for black and white players on the second row of the LCD.
 * Time is in the format mm:ss.t
 * ### Examples
 * ```
 * 15:30.0  15:30.0
 * 00:03.4  04:59.9
 * ```
 */
void printTimes()
{
    lcd.setCursor(0, 1);
    char buffer[17];
    sprintf(buffer, "%02d:%02d.%d  %02d:%02d.%d", 
        black.getMinutes(), black.getSeconds(), black.getTenths(), 
        white.getMinutes(), white.getSeconds(), white.getTenths());
    lcd.print(buffer);
}

void TaskUpdateScreen(void *pvParameters __attribute__((unused)))
{
    for (;;)
    {
        if(blink)
        {
            lcd.clear();
            vTaskDelay(pdMS_TO_TICKS(100));
        }

        lcd.setCursor(0, 0);
        switch (currentState)
        {
        case ClockState::Welcome:
            lcd.print("Chess Clock v1.0");
            lcd.setCursor(0, 1);
            lcd.print(" (c) milo 2024  ");
            vTaskDelay(pdMS_TO_TICKS(1000));
            lcd.setCursor(0, 0);
            lcd.print("\xc1\xaa\xbd \xa5 \xb8\xdb\xaf\xb8  v1.0");
            lcd.setCursor(0, 1);
            lcd.print(" (c)  \xd0\xdb  2024    ");
            vTaskDelay(pdMS_TO_TICKS(1000));
            break;

        case ClockState::ModeSet:
            lcd.print("Select mode:    ");
            lcd.setCursor(0, 1);
            switch (timerMode)
            {
                case TimerMode::SuddenDeath:    lcd.print("\x7f Sudden Death \x7e");  break;
                case TimerMode::Hourglass:      lcd.print("\x7f Hourglass    \x7e");  break;
                case TimerMode::Fischer:        lcd.print("\x7f Fischer      \x7e");  break;

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
            lcd.print("         White \x7e");
            printTimes();
            break;

        case ClockState::Black:
            lcd.print("\x7f Black         ");
            printTimes();
            break;
        case ClockState::PausedWithBlack:
            lcd.print("\x7f Black         ");
            printTimes();
            break;

        case ClockState::PausedWithWhite:
            lcd.print("         White \x7e");
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

        vTaskDelay(pdMS_TO_TICKS(100));
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
                vTaskDelay(pdMS_TO_TICKS(3000));
                currentState = ClockState::ModeSet;
                break;
            
            case ClockState::MinuteSet:
                switch(timerMode)
                {
                    case TimerMode::SuddenDeath:
                    case TimerMode::Hourglass:
                        incrementSeconds = 0;
                        break;

                    case TimerMode::Fischer:
                        incrementSeconds = 30;
                        break;

                    default:
                        break;
                }
                break;
            
            case ClockState::Ready:
                black.setMinutes(playtimeMinutes);
                white.setMinutes(playtimeMinutes);
                black.setIncrement(incrementSeconds);
                white.setIncrement(incrementSeconds);
                break;

            case ClockState::White:
                blink = white.isInDanger();
                white.decrement();
                if(timerMode == TimerMode::Hourglass)
                {
                    black.incrementOneTick();
                }

                if(white.isOutOfTime())
                {
                    currentState = ClockState::GameOverBlackWins;
                }
                break;

            case ClockState::Black:
                blink = black.isInDanger();
                black.decrement();
                if(timerMode == TimerMode::Hourglass)
                {
                    white.incrementOneTick();
                }

                if(black.isOutOfTime())
                {
                    currentState = ClockState::GameOverWhiteWins;
                }
                break;
            
            case ClockState::PausedWithBlack:
            case ClockState::PausedWithWhite:
                blink = true;
                break;
            case ClockState::GameOverBlackWins:
            case ClockState::GameOverWhiteWins:
                blink = false;
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
        // Read button value. Debounce by averaging 2 readings.
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
                        switch (timerMode)
                        {
                            case TimerMode::SuddenDeath:
                            case TimerMode::Hourglass:
                                currentState = ClockState::Ready;
                                break;

                            default:
                                currentState = ClockState::SecondSet;
                                break;
                        }
                        break;

                    case ButtonState::Left:
                        if(playtimeMinutes > 60) playtimeMinutes = 60;
                        else if(playtimeMinutes > 15) playtimeMinutes -= 15;
                        else if(playtimeMinutes > 5) playtimeMinutes -= 5;
                        else if(playtimeMinutes > 1) playtimeMinutes -= 1;
                        break;

                    case ButtonState::Right:
                        if(playtimeMinutes < 5) playtimeMinutes += 1;
                        else if(playtimeMinutes < 15) playtimeMinutes += 5;
                        else if(playtimeMinutes < 60) playtimeMinutes += 15;
                        else if(playtimeMinutes < 90 ) playtimeMinutes = 90;
                        break;
                
                    default:
                        break;
                }
                break;

            // Second set screen:
            // * Select button sets both players minutes to playtimeMinutes and continues to next screen
            // * Left button decrements seconds 5 at a time
            // * Right button increments seconds 5 at a time
            case ClockState::SecondSet:
                switch (button)
                {
                    case ButtonState::Select:
                        currentState = ClockState::Ready;
                        break;

                    case ButtonState::Left:
                        incrementSeconds = (incrementSeconds > 0) ? incrementSeconds - 5 : 0;
                        break;

                    case ButtonState::Right:
                        incrementSeconds = (incrementSeconds < 60) ? incrementSeconds + 5 : 60;
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
                        currentState = ClockState::White;
                        break;

                    case ButtonState::Right:
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
                        if(timerMode == TimerMode::Fischer)
                        {
                            white.increment();
                        }
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
                        if(timerMode == TimerMode::Fischer)
                        {
                            black.increment();
                        }
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
