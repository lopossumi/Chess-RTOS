#pragma once
#include <Arduino.h>

void TaskIOController(void *pvParameters);
void turnLeds(uint8_t value);
void playNote(int freq, int duration);
void playHappyTone();