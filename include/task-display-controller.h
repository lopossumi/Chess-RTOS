#pragma once

#include "enums.h"

void printMinutesOnly(int minutes);
void printSetpoints(int minutes, int seconds);
void printTimesWithoutTenths(long blackTicks, long whiteTicks);
void printTimes(long blackTicks, long whiteTicks, int blackDelay, int whiteDelay);
const char* getHeader(MenuItem menuItem);
const char* getTimerModeName(TimerMode mode);
void TaskDisplayController(void *pvParameters);
void initializeBarDisplay();
void getHeaderRow(Game *game, char *buffer);