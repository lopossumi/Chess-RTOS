#ifndef MAIN_H
#define MAIN_H

enum class TimerMode
{
    SuddenDeath = 0,
    Fischer = 1,
    Hourglass = 2,
    SimpleDelay = 3,
    TimerMode_MAX = 4
};

enum class ButtonState
{
    None,
    Select,
    Left,
    Right,
    ButtonState_MAX
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

struct Timer
{
    int time_minutes;
    int inc_seconds;
};

#endif // MAIN_H