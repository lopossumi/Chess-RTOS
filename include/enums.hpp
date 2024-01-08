#pragma once

enum class PlayerColor
{
    White,
    Black,
    PlayerColor_MAX
};

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
