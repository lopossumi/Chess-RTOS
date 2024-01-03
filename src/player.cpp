class Player
{
private:
    int ticksLeft;

public:
    Player() { ticksLeft = 0; };

    void setMinutes(int minutes)
    {
        ticksLeft = minutes * 600;
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

    void increment(int seconds)
    {
        ticksLeft += seconds * 10;
    }

    bool isOutOfTime()
    {
        return ticksLeft == 0;
    }
};
