#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include "game-state.h"
#include "task-display-controller.h"
#include "text-resources.h"
#include "bar-display.h"
#include "enums.h"
#include "pinout.h"

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);
char buffer[17] = "";

void TaskDisplayController(void *pvParameters)
{
    lcd.init();
    lcd.backlight();
    initializeBarDisplay();

    // Show welcome animation
    lcd.setCursor(0, 0);
    lcd.print(WELCOME1);
    lcd.setCursor(0, 1);
    lcd.print(WELCOME2);
    vTaskDelay(pdMS_TO_TICKS(1000));
    lcd.setCursor(0, 0);
    lcd.print(WELCOME3);
    lcd.setCursor(0, 1);
    lcd.print(WELCOME4);
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    for (;;)
    {
        Game *game = static_cast<Game *>(pvParameters);

        if (game->isBlinking())
        {
            lcd.noDisplay();
            vTaskDelay(pdMS_TO_TICKS(100));
            lcd.display();
        }

        // Print header only if it has changed to save cycles
        if (game->updateHeader)
        {
            lcd.setCursor(0, 0);
            if (game->isMenuOpen)
            {
                lcd.print(getHeader(game->menuItem));
            }
            else if (!game->isGameStarted)
            {
                lcd.print(READY_TO_START);
            }
            else if (game->isGameOver)
            {
                lcd.print(game->isBlackTurn ? WHITE_WINS : BLACK_WINS);
            }
            else
            {
                lcd.print(game->isBlackTurn ? BLACK_TO_PLAY : WHITE_TO_PLAY);
            }

            game->updateHeader = false;
        }

        lcd.setCursor(0, 1);
        if (game->isMenuOpen)
        {
            switch (game->menuItem)
            {
            case MenuItem::Mode:
                lcd.print(getTimerModeName(game->timerMode));
                break;
            case MenuItem::Minutes:
                printMinutesOnly(game->playtimeMinutes);
                break;
            case MenuItem::Increment:
                printSetpoints(game->playtimeMinutes, game->incrementSeconds);
                break;
            default:
                break;
            }
        }
        else
        {
            printTimes(game->blackTicksLeft, game->whiteTicksLeft, game->getBlackDelayBar(), game->getWhiteDelayBar());
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

void printTimesWithoutTenths(long blackTicks, long whiteTicks)
{
    snprintf(&buffer[0], 17, "%d:%02d           ",
             (int)(blackTicks / 600),
             (int)(blackTicks / 10 % 60));

    snprintf(&buffer[10], 7, "%3d:%02d",
             (int)(whiteTicks / 600),
             (int)(whiteTicks / 10 % 60));

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
void printTimes(long blackTicks, long whiteTicks, int blackDelay, int whiteDelay)
{

    if (blackTicks / 600 > 99 || whiteTicks / 600 > 99)
    {
        printTimesWithoutTenths(blackTicks, whiteTicks);
        return;
    }

    lcd.setCursor(0, 1);
    snprintf(&buffer[0], 8, "%d:%02d.%d   ",
             (int)(blackTicks / 600),
             (int)(blackTicks / 10 % 60),
             (int)(blackTicks % 10));
    lcd.print(buffer);

    lcd.write(byte(blackDelay));
    lcd.write(byte(whiteDelay));

    lcd.setCursor(9, 1);
    snprintf(&buffer[0], 8, "%2d:%02d.%d",
             (int)(whiteTicks / 600),
             (int)(whiteTicks / 10 % 60),
             (int)(whiteTicks % 10));
    lcd.print(buffer);
}

const char* getHeader(MenuItem menuItem)
{
    switch (menuItem)
    {
    case MenuItem::Mode:
        return SELECT_MODE;
    case MenuItem::Minutes:
        return SET_MINUTES;
    case MenuItem::Increment:
        return SET_SECONDS;
    default:
        return UNKNOWN;
    }
}

const char *getTimerModeName(TimerMode mode)
{
    switch (mode)
    {
    case TimerMode::SuddenDeath:
        return SUDDEN_DEATH;
    case TimerMode::Hourglass:
        return HOURGLASS;
    case TimerMode::Fischer:
        return FISCHER;
    case TimerMode::SimpleDelay:
        return SIMPLE_DELAY;
    default:
        return UNKNOWN;
    }
}
