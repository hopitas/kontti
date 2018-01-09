
#include "StandaloneControlForWaterPump.h"

bool StandaloneControlForWaterPump::shouldStartPump(timeInMilliseconds currentTime)
{
	return
		waterLowSensor->HasWater() &&
		!waterHighSensor->HasWater() &&
		isTimeToStartPump(currentTime);
}

bool StandaloneControlForWaterPump::isTimeToStartPump(timeInMilliseconds currentTime)
{
	return waterPump->lastStartTime == 0 ||
		currentTime > waterPump->lastStartTime + cycleDuration - onDuration;
}

bool StandaloneControlForWaterPump::shouldStopPump(timeInMilliseconds currentTime)
{
	return !waterLowSensor->HasWater() ||
		waterHighSensor->HasWater() ||
		isTimeToStopPump(currentTime);
}

bool StandaloneControlForWaterPump::isTimeToStopPump(timeInMilliseconds currentTime)
{
	return currentTime > waterPump->lastStartTime + onDuration;
}


void StandaloneControlForWaterPump::cycle(timeInMilliseconds currentTime)
{
	if (waterPump->pumpIsOn)
	{
		if (shouldStopPump(currentTime))
		{
			waterPump->stop(currentTime);
		}
	}
	else
	{
		if (shouldStartPump(currentTime))
		{
			waterPump->start(currentTime);
		}
	}
}

void StandaloneControlForWaterPump::pleaseStartNow(timeInMilliseconds currentTime)
{
	waterPump->lastStartTime = currentTime - cycleDuration - 1;
	cycle(currentTime);
}
