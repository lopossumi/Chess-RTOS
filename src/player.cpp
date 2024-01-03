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
    };

    void initialize(int minutes, int increment)
    {
        ticksLeft = minutes * 600l;
        incrementAmount = increment * 10;
        delay = incrementAmount;
    }

    PlayerColor getColor()  { return color;                         }
    int getMinutes()        { return ticksLeft / 600;               }
    int getSeconds()        { return (ticksLeft / 10) % 60;         }
    int getTenths()         { return ticksLeft % 10;                }
    bool isOutOfTime()      { return ticksLeft == 0;                }
    bool isInDanger()       { return ticksLeft <= 100;              }
    bool isBlack()          { return color == PlayerColor::Black;   }
    bool isWhite()          { return color == PlayerColor::White;   }
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
