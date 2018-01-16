
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

WaterPumpStopReason StandaloneControlForWaterPump::shouldStopPump(timeInMilliseconds currentTime)
{
	return !waterLowSensor->HasWater() ? LOWER_CONTAINER_EMPTY :
		waterHighSensor->HasWater() ? UPPER_CONTAINER_FULL :
		isTimeToStopPump(currentTime) ? TIMEOUT :
		CONTINUE;
}

bool StandaloneControlForWaterPump::isTimeToStopPump(timeInMilliseconds currentTime)
{
	return currentTime > waterPump->lastStartTime + onDuration;
}


void StandaloneControlForWaterPump::cycle(timeInMilliseconds currentTime)
{
	if (waterPump->pumpIsOn)
	{
		WaterPumpStopReason whyShouldStop = shouldStopPump(currentTime);
		if (whyShouldStop != CONTINUE)
		{
			lastStopReason = whyShouldStop;
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
