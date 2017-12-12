// StandaloneControlForWaterPump.h

#ifndef _StandaloneControlForWATERPUMP_h
#define _StandaloneControlForWATERPUMP_h
#include "WaterPump.h"

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

typedef unsigned long timeInMilliseconds;

class StandaloneControlForWaterPump
{
public:
	StandaloneControlForWaterPump(WaterPump waterPump, timeInMilliseconds offDuration, timeInMilliseconds onDuration);
	void cycle(timeInMilliseconds currentTime);
};

#endif

