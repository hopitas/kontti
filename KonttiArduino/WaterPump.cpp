
#include "WaterPump.h"

void WaterPump::start(timeInMilliseconds currentTime)
{
	digitalWrite(pin_number, HIGH);
	lastStartTime = currentTime;
	pumpIsOn = true;
}

void WaterPump::stop(timeInMilliseconds currentTime)
{
	digitalWrite(pin_number, LOW);
	pumpIsOn = false;
}
