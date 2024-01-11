#pragma once

#include "enums.h"

void TaskDisplayController(void *pvParameters);

void printMinutesOnly(int minutes);
void printSetpoints(int minutes, int seconds);
void printTimesWithoutTenths(long blackTicks, long whiteTicks);
void printTimes(long blackTicks, long whiteTicks, int blackDelay, int whiteDelay);
const char* getHeader(MenuItem menuItem);
const char* getTimerModeName(TimerMode mode);
void initializeBarDisplay();
void getHeaderRow(Game *game, char *buffer);