// WaterPump.h

#ifndef _WATERPUMP_h
#define _WATERPUMP_h

#include "Arduino.h"

typedef unsigned long timeInMilliseconds;

class WaterPump
{

private:
	int pin_number;

public:
	timeInMilliseconds lastStartTime;
	bool pumpIsOn;

	void start(timeInMilliseconds currentTime);
	void stop(timeInMilliseconds currentTime);

	WaterPump(int pin_number)
	{
		this->pin_number = pin_number;
		this->lastStartTime = 0;
		this->pumpIsOn = false;
	};
};

#endif
