#pragma once

#include "enums.hpp"
#include "Player.hpp"

void printMinutesOnly(int minutes);
void printSetpoints(int minutes, int seconds);
void printTimesWithoutTenths(Player black, Player white);
void printTimes(Player black, Player white);
const char* getHeader(ClockState state, bool isBlack);
const char* getTimerModeName(TimerMode mode);
void TaskUpdateScreen(void *pvParameters);
void initializeBarDisplay();