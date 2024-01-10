#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include "enums.hpp"
#include "gameState.h"

#include "taskUpdateScreen.hpp"
#include "taskReadButtons.h"
#include "taskGameLoop.h"

void TaskUpdateScreen(void *pvParameters);
void TaskReadButton(void *pvParameters);
void TaskGameLoop(void *pvParameters);

Game gameState = Game();

void setup()
{
    TimerMode timerMode = (TimerMode)EEPROM.read(0);
    int playtimeMinutes = EEPROM.read(1);
    int incrementSeconds = EEPROM.read(2);

    gameState.initialize(timerMode, playtimeMinutes, incrementSeconds);
    
    // FreeRTOS  Function Name      Task Name       Stack   Params  Prio  Handle
    xTaskCreate( TaskUpdateScreen,  "UpdateScreen", 128,    (void*)&gameState,   2,    NULL );
    xTaskCreate( TaskReadButtons,   "ReadButton",   128,    (void*)&gameState,   1,    NULL );
    xTaskCreate( TaskGameLoop,      "GameLoop",     128,    (void*)&gameState,   3,    NULL );

    vTaskStartScheduler();
}

void loop() {}
