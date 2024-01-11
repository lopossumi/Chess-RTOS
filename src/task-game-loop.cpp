#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include "game-state.h"
#include "pinout.h"


void TaskGameLoop(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
        xLastWakeTime = xTaskGetTickCount();

        Game *gameState = static_cast<Game *>(pvParameters);
        if (gameState->isGameStarted && !gameState->isPaused && !gameState->isGameOver)
        {
            gameState->update();
        }
    }
}