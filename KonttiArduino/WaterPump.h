// WaterPump.h

#ifndef _WATERPUMP_h
#define _WATERPUMP_h

#include "Arduino.h"

typedef unsigned long timeInMilliseconds;

class WaterPump
{
public:
	timeInMilliseconds lastStartTime, lastEndTime;
	bool pumpIsOn;

	WaterPump(int pin_number);
	void start(timeInMilliseconds currentTime);
	void stop(timeInMilliseconds currentTime);
};

#endif
