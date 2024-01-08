#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <LiquidCrystal.h>
#include <Wire.h>

#include "gameState.hpp"
#include "taskUpdateScreen.hpp"
#include "textResources.hpp"
#include "barDisplay.hpp"
#include "enums.hpp"
#include "Player.hpp"

const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
char buffer[17] = "";

void TaskUpdateScreen(void *pvParameters)
{
    initializeBarDisplay();
    lcd.begin(16, 2);

    // Show welcome animation
    lcd.setCursor(0, 0); lcd.print(WELCOME1);
    lcd.setCursor(0, 1); lcd.print(WELCOME2);
    vTaskDelay(pdMS_TO_TICKS(1000));
    lcd.setCursor(0, 0); lcd.print(WELCOME3);
    lcd.setCursor(0, 1); lcd.print(WELCOME4);
    vTaskDelay(pdMS_TO_TICKS(1000));

    for (;;)
    {
        GameState* gameState = static_cast<GameState*>(pvParameters);

        bool blink = gameState->isPausedNow()
            || gameState->isBlackInDanger()
            || gameState->isWhiteInDanger();
        if(blink)
        {
            lcd.noDisplay();
            vTaskDelay(pdMS_TO_TICKS(100));
            lcd.display();
        }

        // Print header
        lcd.setCursor(0, 0);
        lcd.print(getHeader(gameState->getCurrentMenuItem(), gameState->isBlackTurnNow()));

        lcd.setCursor(0, 1);
        if (gameState->isMenuOpenNow())
        {
            switch (gameState->getCurrentMenuItem())
            {
            case MenuItem::Mode:
                lcd.print(getTimerModeName(gameState->getTimerMode()));
                break;
            case MenuItem::MinuteSet:
                printMinutesOnly(gameState->getPlayTimeMinutes());
                break;
            case MenuItem::SecondSet:
                printSetpoints(gameState->getPlayTimeSeconds());
                break;
            }
        }
        else
        {
            printTimes(gameState->getBlackPlayer(), gameState->getWhitePlayer());
        }
        {


        case ClockState::Ready:
        case ClockState::Play:
        case ClockState::Pause:
        case ClockState::GameOver:
            printTimes(black, white);
            break;

        default:
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void initializeBarDisplay()
{
  lcd.createChar(0, bar0);
  lcd.createChar(1, bar1);
  lcd.createChar(2, bar2);
  lcd.createChar(3, bar3);
  lcd.createChar(4, bar4);
  lcd.createChar(5, bar5);
  lcd.createChar(6, bar6);
  lcd.createChar(7, bar7);
}


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
void printSetpoints(int minutes, int seconds)
{
    snprintf(buffer, 17, "%6dm + %02ds   ", minutes, seconds);
    lcd.print(buffer);
}

void printTimesWithoutTenths(Player black, Player white)
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
void printMinutesOnly(int minutes)
{
    snprintf(buffer, 17, "%8dm       ", minutes);
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
void printTimes(Player black, Player white)
{
    if(black.getMinutes() > 99 || white.getMinutes() > 99)
    {
        printTimesWithoutTenths(black, white);
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
