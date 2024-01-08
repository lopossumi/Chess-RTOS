#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <Wire.h>

#include "enums.hpp"
#include "barDisplay.hpp"
#include "player.hpp"
#include "main.hpp"

const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void TaskUpdateScreen(void *pvParameters);
void TaskReadButton(void *pvParameters);
void TaskGameLoop(void *pvParameters);

ClockState currentState;

TimerMode timerMode;

Player black = Player(PlayerColor::Black);
Player white = Player(PlayerColor::White);
Player *currentPlayer;
Player *otherPlayer;

int playtimeMinutes;
int incrementSeconds;
bool blink;
char buffer[17] = "";

void setup()
{
    initializeBarDisplay(lcd);

    timerMode = (TimerMode)EEPROM.read(0);
    playtimeMinutes = EEPROM.read(1);
    incrementSeconds = EEPROM.read(2);

    lcd.begin(16, 2);
    currentState = ClockState::Welcome;

    // FreeRTOS  Function Name      Task Name       Stack   Params  Prio  Handle
    xTaskCreate( TaskUpdateScreen,  "UpdateScreen", 256,    NULL,   2,    NULL );
    xTaskCreate( TaskReadButton,    "ReadButton",   128,    NULL,   1,    NULL );
    xTaskCreate( TaskGameLoop,      "GameLoop",     128,    NULL,   3,    NULL );

    vTaskStartScheduler();
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
    snprintf(buffer, 17, "%6dm + %02ds   ", playtimeMinutes, incrementSeconds);
    lcd.print(buffer);
}

void printTimesWithoutTenths()
{
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
    snprintf(&buffer[0], 8, "%d:%02d.%d   ",
        black.getMinutes(),
        black.getSeconds(),
        black.getTenths());
    lcd.print(buffer);

    lcd.write(byte(black.getDelayBar()));
    lcd.write(byte(white.getDelayBar()));
    
    lcd.setCursor(9, 1);
    snprintf(&buffer[0], 8, "%2d:%02d.%d",
        white.getMinutes(),
        white.getSeconds(),
        white.getTenths());
    lcd.print(buffer);
}

const char* getHeader(ClockState state, bool isBlack)
{
    switch (state)
    {
        case ClockState::Welcome:       return WELCOME1;
        case ClockState::ModeSet:       return SELECT_MODE;
        case ClockState::MinuteSet:     return SET_MINUTES;
        case ClockState::SecondSet:     return SET_SECONDS;
        case ClockState::Ready:         return READY_TO_START;
        case ClockState::Play:          return isBlack ? BLACK_TO_PLAY : WHITE_TO_PLAY;
        case ClockState::Pause:         return isBlack ? BLACK_TO_PLAY : WHITE_TO_PLAY;
        case ClockState::GameOver:      return isBlack ? WHITE_WINS : BLACK_WINS;
        default:                        return "";
    }
}

const char* getTimerModeName(TimerMode mode)
{
    switch (mode)
    {
        case TimerMode::SuddenDeath:    return SUDDEN_DEATH;
        case TimerMode::Hourglass:      return HOURGLASS;
        case TimerMode::Fischer:        return FISCHER;
        case TimerMode::SimpleDelay:    return SIMPLE_DELAY;
        default:                        return UNKNOWN;
    }
}

void TaskUpdateScreen(void *pvParameters __attribute__((unused)))
{
    // Show welcome animation
    lcd.setCursor(0, 0); lcd.print(WELCOME1);
    lcd.setCursor(0, 1); lcd.print(WELCOME2);
    vTaskDelay(pdMS_TO_TICKS(1000));
    lcd.setCursor(0, 0); lcd.print(WELCOME3);
    lcd.setCursor(0, 1); lcd.print(WELCOME4);
    vTaskDelay(pdMS_TO_TICKS(1000));

    for (;;)
    {
        if(blink)
        {
            lcd.noDisplay();
            vTaskDelay(pdMS_TO_TICKS(100));
            lcd.display();
        }

        // Print header
        lcd.setCursor(0, 0);
        lcd.print(getHeader(currentState, currentPlayer->isBlack()));

        lcd.setCursor(0, 1);
        switch (currentState)
        {
            case ClockState::ModeSet:       lcd.print(getTimerModeName(timerMode));     break;
            case ClockState::MinuteSet:     printMinutesOnly();                         break;
            case ClockState::SecondSet:     printSetpoints();                           break;
            case ClockState::Ready:         printTimes();                               break;
            case ClockState::Play:          printTimes();                               break;
            case ClockState::Pause:         printTimes();                               break;
            case ClockState::GameOver:      printTimes();                               break;

            default:
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

Button convertButtonState(int value)
{
    switch (value)
    {
        case 1000 ... 1023: return Button::None;
        case 500 ... 699:   return Button::Select;
        case 300 ... 499:   return Button::Left;
        case 0 ... 49:      return Button::Right;
        default:            return Button::None;
    }
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
        int reading = analogRead(0);
        int value = reading;
        while (reading < 1000)
        {
            value += reading;
            value /= 2;
            reading = analogRead(0);
            vTaskDelay(1);
        }
        auto button = convertButtonState(value);
        if(button == Button::None) continue;

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
                    case Button::Select:
                        currentState = ClockState::MinuteSet;
                        break;

                    case Button::Left:
                        timerMode = (timerMode <= (TimerMode)0) 
                            ? (TimerMode)((int)TimerMode::TimerMode_MAX - 1) 
                            : (TimerMode)((int)timerMode - 1);
                        break;

                    case Button::Right:
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
            // * Minute options are 1, 2, 3, 4, 5, 10, 15, 20, 25, 30, 45, 60, 90
            case ClockState::MinuteSet:
                switch (button)
                {
                    case Button::Select:
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

                    case Button::Left:
                        switch (playtimeMinutes)
                        {
                            case 61 ... 90: playtimeMinutes = 60;   break;
                            case 31 ... 60: playtimeMinutes -= 15;  break;
                            case 6  ... 30: playtimeMinutes -= 5;   break;
                            case 2  ...  5: playtimeMinutes -= 1;   break;
                            default:
                                break;
                        }
                        break;

                    case Button::Right:
                        switch(playtimeMinutes)
                        {
                            case 1  ...  4: playtimeMinutes += 1;   break;
                            case 5  ... 29: playtimeMinutes += 5;   break;
                            case 30 ... 59: playtimeMinutes += 15;  break;
                            case 60 ... 90: playtimeMinutes = 90;   break;
                            default:
                                break;
                        }
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
                    case Button::Select:
                        currentState = ClockState::Ready;
                        break;

                    case Button::Left:
                        incrementSeconds = (incrementSeconds > 0) ? incrementSeconds - 5 : 0;
                        break;

                    case Button::Right:
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
                    case Button::Select:
                        EEPROM.write(0, (int)timerMode);
                        EEPROM.write(1, playtimeMinutes);
                        EEPROM.write(2, incrementSeconds);
                        break;
                    case Button::Left:
                        currentPlayer = &white;
                        otherPlayer = &black;
                        currentState = ClockState::Play;                        
                        break;

                    case Button::Right:
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
                    case Button::Select:
                        blink = true;
                        currentState = ClockState::Pause;
                        break;
                    
                    case Button::Left:
                    case Button::Right:
                        if (button == Button::Left && currentPlayer->isWhite()) break;
                        if (button == Button::Right && currentPlayer->isBlack()) break;

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
                        
                        {
                            Player *p = currentPlayer;
                            currentPlayer = otherPlayer;
                            otherPlayer = p;
                        }
                        
                        break;
                    
                    default:
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
                    case Button::Select:
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
                    case Button::Select:
                        currentState = ClockState::ModeSet;
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
