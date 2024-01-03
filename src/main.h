#ifndef MAIN_H
#define MAIN_H

enum class TimerMode
{
    SuddenDeath = 0,
    Fischer = 1,
    Hourglass = 2,
    TimerMode_MAX = 3
};

enum class ButtonState
{
    None,
    Hold,
    Select,
    Left,
    Right,
    Up,
    Down,
    ButtonState_MAX
};

enum class ClockState
{
    Welcome,
    ModeSet,
    MinuteSet,
    SecondSet,
    Ready,
    White,
    Black,
    PausedWithWhite,
    PausedWithBlack,
    GameOverWhiteWins,
    GameOverBlackWins,
    ClockState_MAX
};

struct Timer
{
    int time_minutes;
    int inc_seconds;
};

#endif // MAIN_H