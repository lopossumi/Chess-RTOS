#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include "pinout.h"
#include "enums.h"
#include "game-state.h"

#include "task-display-controller.h"
#include "task-read-buttons.h"
#include "task-game-loop.h"
#include "task-led-controller.h"

void TaskUpdateScreen(void *pvParameters);
void TaskReadButton(void *pvParameters);
void TaskGameLoop(void *pvParameters);

Game gameState = Game(
    (TimerMode)EEPROM.read(EEPROM_TIMER_MODE),
    (int)EEPROM.read(EEPROM_PLAYTIME_MINUTES),
    (int)EEPROM.read(EEPROM_INCREMENT_SECONDS)
    );

void setup()
{   
    // FreeRTOS  Function Name          Task Name               Stack   Params              Prio  Handle
    xTaskCreate( TaskDisplayController, "Lcd controller",       128,    (void*)&gameState,  2,    NULL );
    xTaskCreate( TaskReadButtons,       "Button Reader",        60,     (void*)&gameState,  4,    NULL );
    xTaskCreate( TaskGameLoop,          "Game loop",            60,     (void*)&gameState,  3,    NULL );
    xTaskCreate( TaskLedController,     "Led controller",       60,     (void*)&gameState,  5,    NULL );
    vTaskStartScheduler();
}

void loop() {}
