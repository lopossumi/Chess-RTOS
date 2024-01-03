class Player
{
private:
    long ticksLeft;
    int incrementAmount;

public:
    Player() { ticksLeft = 0; };

    void setMinutes(int minutes)
    {
        ticksLeft = minutes * 600l;
    }

    void setIncrement(int seconds)
    {
        incrementAmount = seconds * 10;
    }

    int getMinutes() 
    { 
        return ticksLeft / 600; 
    }

    int getSeconds()
    {
        return (ticksLeft / 10) % 60;
    }

    int getTenths()
    {
        return ticksLeft % 10;
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

    bool isOutOfTime()
    {
        return ticksLeft == 0;
    }

    bool isInDanger()
    {
        return ticksLeft <= 100;
    }
};
