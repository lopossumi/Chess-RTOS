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
    Seconds,
    MenuItem_MAX
};

enum class ClockState
{
    Welcome,
    ModeSet,
    MinuteSet,
    SecondSet,
    Ready,
    Play,
    Pause,
    GameOver,
    ClockState_MAX
};
