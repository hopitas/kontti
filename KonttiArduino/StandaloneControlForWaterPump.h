// StandaloneControlForWaterPump.h

#ifndef _StandaloneControlForWATERPUMP_h
#define _StandaloneControlForWATERPUMP_h
#include "WaterPump.h"
#include "WaterLevelSensor.h"

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

typedef unsigned long timeInMilliseconds;

class StandaloneControlForWaterPump
{
public:
	StandaloneControlForWaterPump(WaterPump *waterPump, WaterLevelSensor *waterLowSensor, WaterLevelSensor *waterHighSensor, timeInMilliseconds offDuration, timeInMilliseconds onDuration)
	{
		this->waterPump = waterPump;
		this->waterLowSensor = waterLowSensor;
		this->waterHighSensor = waterHighSensor;
		this->cycleDuration = offDuration;
		this->onDuration = onDuration;
	};

	bool shouldStartPump(timeInMilliseconds currentTime);

	bool shouldStopPump(timeInMilliseconds currentTime);

	void cycle(timeInMilliseconds currentTime);

	void pleaseStartNow(timeInMilliseconds currentTime);

private:
	WaterPump *waterPump;
	WaterLevelSensor *waterLowSensor, *waterHighSensor;
	timeInMilliseconds cycleDuration;
	timeInMilliseconds onDuration;

	bool isTimeToStartPump(timeInMilliseconds currentTime);
	bool isTimeToStopPump(timeInMilliseconds currentTime);
};

#endif

