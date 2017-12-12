
#include "WaterPump.h"

class WaterPump
{
private:
	int pin_number;

public:
	WaterPump(int pin_number)
	{
		this->pin_number = pin_number;
		this->lastStartTime = 0;
		this->lastEndTime = 0;
		this->pumpIsOn = false;
	};

	void start(timeInMilliseconds currentTime)
	{
		digitalWrite(pin_number, HIGH);
		lastStartTime = currentTime;
		pumpIsOn = true;
	}

	void stop(timeInMilliseconds currentTime)
	{
		digitalWrite(pin_number, LOW);
		lastEndTime = currentTime;
		pumpIsOn = false;
	}
};
