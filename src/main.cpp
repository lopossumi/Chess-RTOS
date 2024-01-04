#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include <EEPROM.h>
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

Player black(PlayerColor::Black);
Player white(PlayerColor::White);
Player *currentPlayer;
Player *otherPlayer;

int playtimeMinutes;
int incrementSeconds;
bool blink;
char buffer[17] = "";

void setup()
{
    timerMode = (TimerMode)EEPROM.read(0);
    playtimeMinutes = EEPROM.read(1);
    incrementSeconds = EEPROM.read(2);

    lcd.begin(16, 2);
    currentState = ClockState::Welcome;

    // FreeRTOS  Function Name      Task Name       Stack   Params  Prio  Handle
    xTaskCreate( TaskUpdateScreen,  "UpdateScreen", 256,    NULL,   2,    NULL );
    xTaskCreate( TaskReadButton,    "ReadButton",   128,    NULL,   1,    NULL );
    xTaskCreate( TaskGameLoop,      "GameLoop",     128,    NULL,   3,    NULL );
}

void loop() {}

/** 
 * Print setpoints for minutes and seconds on the second row of the LCD.
 * Time is in the format m + ss
 * 
 * ### Examples ###
 * ```
 *     15m + 30s   
 *     5m + 00s   
 *     90m + 00s   
 * ```
 */
void printSetpoints()
{
    lcd.setCursor(0, 1);
    snprintf(buffer, 17, "%6dm + %02ds   ", playtimeMinutes, incrementSeconds);
    lcd.print(buffer);
}

void printTimesWithoutTenths()
{
    lcd.setCursor(0, 1);
    snprintf(&buffer[0], 17, "%d:%02d           ",
        black.getMinutes(),
        black.getSeconds());

    snprintf(&buffer[10], 7, "%3d:%02d",
        white.getMinutes(),
        white.getSeconds());

    lcd.print(buffer);
}

/** 
 * Print minutes only on the second row of the LCD.
 * Time is padded with spaces to 8 characters.
 * ### Examples ###
 * ```
 * 15m         
 * 5m         
 * 90m         
 * ```
 */
void printMinutesOnly()
{
    lcd.setCursor(0, 1);
    snprintf(buffer, 17, "%8dm       ", playtimeMinutes);
    lcd.print(buffer);
}

/** 
 * Print times for black and white players on the second row of the LCD.
 * Time is in the format mm:ss.t
 * 
 * Todo: Handle times greater than 99 minutes.
 * 
 * ### Examples
 * ```
 * 15:30.0  15:30.0
 * 00:03.4  04:59.9
 * ```
 */
void printTimes()
{
    if(black.getMinutes() > 99 || white.getMinutes() > 99)
    {
        printTimesWithoutTenths();
        return;
    }

    lcd.setCursor(0, 1);
    snprintf(&buffer[0], 17, "%d:%02d.%d         ",
        black.getMinutes(),
        black.getSeconds(),
        black.getTenths());

    snprintf(&buffer[9], 8, "%2d:%02d.%d",
        white.getMinutes(),
        white.getSeconds(),
        white.getTenths());

    lcd.print(buffer);
}

void TaskUpdateScreen(void *pvParameters __attribute__((unused)))
{
    for (;;)
    {
        if(blink)
        {
            lcd.noDisplay();
            vTaskDelay(pdMS_TO_TICKS(100));
            lcd.display();
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
                case TimerMode::SuddenDeath:    lcd.print("\x7f Sudden Death \x7e"); break;
                case TimerMode::Hourglass:      lcd.print("\x7f Hourglass    \x7e"); break;
                case TimerMode::Fischer:        lcd.print("\x7f Fischer      \x7e"); break;
                case TimerMode::SimpleDelay:    lcd.print("\x7f Simple Delay \x7e"); break;
                default:                        lcd.print("\x7f Unknown      \x7e"); break;
            }
            break;

        case ClockState::MinuteSet:
            lcd.print("Set minutes:    ");
            printMinutesOnly();
            break;

        case ClockState::SecondSet:
            lcd.print("Set increment:  ");
            printSetpoints();
            break;

        case ClockState::Ready:
            lcd.print(" Ready to Start ");
            printTimes();
            break;

        case ClockState::Play:
        case ClockState::Pause:
            if(currentPlayer->isBlack())    { lcd.print("\x7f Black         "); }
            else                            { lcd.print("         White \x7e"); }
            printTimes();
            break;

        case ClockState::GameOver:
            lcd.print("   Game Over    ");
            lcd.setCursor(0, 1);
            if(currentPlayer->isBlack())    { lcd.print("   White wins! \x7e"); }
            else                            { lcd.print("\x7f  Black wins!  "); }
            break;

        default:
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

ButtonState convertButtonState(int value)
{
    if (value >= 1000)      return ButtonState::None;
    else if (value < 50)    return ButtonState::Right;
    else if (value < 150)   return ButtonState::None;
    else if (value < 300)   return ButtonState::None;
    else if (value < 500)   return ButtonState::Left;
    else if (value < 700)   return ButtonState::Select;
    else                    return ButtonState::None;   
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
                        break;

                    default:
                        break;
                }
                break;
            
            case ClockState::Ready:
                break;

            case ClockState::Play:
                blink = currentPlayer->isInDanger();
                switch(timerMode)
                {
                    case TimerMode::SimpleDelay:
                        currentPlayer->delayOrDecrement();
                        break;

                    case TimerMode::Hourglass:
                        otherPlayer->incrementOneTick();
                        currentPlayer->decrement();
                        break;

                    default:
                        currentPlayer->decrement();
                        break;
                }
                if(currentPlayer->isOutOfTime())
                {
                    currentState = ClockState::GameOver;
                    blink = false;
                }
                break;
            
            case ClockState::Pause:
                blink = true;
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
                        timerMode = (timerMode <= (TimerMode)0) 
                            ? (TimerMode)((int)TimerMode::TimerMode_MAX - 1) 
                            : (TimerMode)((int)timerMode - 1);
                        break;

                    case ButtonState::Right:
                        timerMode = (timerMode >= (TimerMode)((int)TimerMode::TimerMode_MAX - 1)) 
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
                                black.initialize(playtimeMinutes, incrementSeconds);
                                white.initialize(playtimeMinutes, incrementSeconds);
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
                        black.initialize(playtimeMinutes, incrementSeconds);
                        white.initialize(playtimeMinutes, incrementSeconds);
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
                    case ButtonState::Select:
                        EEPROM.write(0, (int)timerMode);
                        EEPROM.write(1, playtimeMinutes);
                        EEPROM.write(2, incrementSeconds);
                        break;
                    case ButtonState::Left:
                        currentPlayer = &white;
                        otherPlayer = &black;
                        currentState = ClockState::Play;                        
                        break;

                    case ButtonState::Right:
                        currentPlayer = &black;
                        otherPlayer = &white;
                        currentState = ClockState::Play;
                        break;
                
                    default:
                        break;
                }
                break;

            // Play screen:
            // * Select button pauses the game
            // * Left button is black player button
            // * Right button is white player button
            case ClockState::Play:
                switch (button)
                {
                    case ButtonState::Select:
                        blink = true;
                        currentState = ClockState::Pause;
                        break;
                    
                    case ButtonState::Left:
                    case ButtonState::Right:
                        if (button == ButtonState::Left && currentPlayer->isWhite()) break;
                        if (button == ButtonState::Right && currentPlayer->isBlack()) break;

                        switch (timerMode)
                        {
                            case TimerMode::SimpleDelay:
                                currentPlayer->resetDelay();
                                break;
                            case TimerMode::Fischer:
                                currentPlayer->increment();
                                break;
                            default:
                                break;
                        }
                        Player *temp = currentPlayer;
                        currentPlayer = otherPlayer;
                        otherPlayer = temp;
                        break;
                }
                break;

            // Pause screen:
            // * Select button resumes the game
            // * Left button does nothing
            // * Right button does nothing
            case ClockState::Pause:
                switch (button)
                {
                    case ButtonState::Select:
                        blink = false;
                        currentState = ClockState::Play;
                        break;
                
                    default:
                        break;
                }
                break;

            // Game over screen:
            // * Select button returns to set minutes screen
            // * Left button does nothing
            // * Right button does nothing
            case ClockState::GameOver:
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
