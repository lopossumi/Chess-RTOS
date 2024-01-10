#pragma once

enum class TimerMode
{
    SuddenDeath = 0,
    Fischer = 1,
    Hourglass = 2,
    SimpleDelay = 3,
    TimerMode_MAX = 4
};

enum class Button
{
    Black,
    White,
    Select,
    Button_MAX
};

enum class MenuItem
{
    Mode,
    Minutes,
    Increment,
    MenuItem_MAX
};
