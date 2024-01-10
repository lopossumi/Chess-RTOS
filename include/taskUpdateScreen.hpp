#pragma once

#include "enums.hpp"
#include "Player.hpp"

void printMinutesOnly(int minutes);
void printSetpoints(int minutes, int seconds);
void printTimesWithoutTenths(long blackTicks, long whiteTicks);
void printTimes(long blackTicks, long whiteTicks);
const char* getHeader(MenuItem menuItem);
const char* getTimerModeName(TimerMode mode);
void TaskUpdateScreen(void *pvParameters);
void initializeBarDisplay();