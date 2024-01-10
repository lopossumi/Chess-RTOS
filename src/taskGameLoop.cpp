#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include "gameState.h"

void TaskGameLoop(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
        xLastWakeTime = xTaskGetTickCount();

        GameState *gameState = static_cast<GameState *>(pvParameters);
        if (gameState->isGameStarted && !gameState->isPaused && !gameState->isGameOver)
        {
            gameState->update();
        }
    }
}