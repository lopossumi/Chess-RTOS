enum class PlayerColor
{
    White,
    Black,
    PlayerColor_MAX
};

class Player
{
private:
    PlayerColor color;
    long ticksLeft;
    int incrementAmount;
    int delay;

public:
    Player(PlayerColor color) 
    {
        this->color = color;
        ticksLeft = 0;
        incrementAmount = 0;
        delay = 0;
    };

    void initialize(int minutes, int increment)
    {
        ticksLeft = minutes * 600l;
        incrementAmount = increment * 10;
        delay = incrementAmount;
    }

    int getMinutes()        { return ticksLeft / 600;               }
    int getSeconds()        { return (ticksLeft / 10) % 60;         }
    int getTenths()         { return ticksLeft % 10;                }
    bool isOutOfTime()      { return ticksLeft == 0;                }
    bool isInDanger()       { return ticksLeft <= 100;              }
    bool isBlack()          { return color == PlayerColor::Black;   }

    bool isWhite()          { return color == PlayerColor::White;   }

    int getDelayBar()
    {
        // The return value is calculated from a fraction of delay / incrementAmount and scaled to 0 - 7.
        if (incrementAmount == 0){ return 0; }
        return (delay * 8) / incrementAmount;
    }

    void decrement()
    {
        if (ticksLeft > 0)
        {
            ticksLeft--;
        }
    }

    void increment()
    {
        ticksLeft += incrementAmount;
    }

    void incrementOneTick()
    {
        ticksLeft++;
    }

    void delayOrDecrement()
    {
        if (delay > 0)
        {
            delay--;
        }
        else
        {
            decrement();
        }
    }

    void resetDelay()
    {
        delay = incrementAmount;
    }
};
