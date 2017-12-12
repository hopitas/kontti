
#include "StandaloneControlForWaterPump.h"

class StandaloneControlForWaterPump
{
private:
	WaterPump waterPump;
	timeInMilliseconds offDuration;
	timeInMilliseconds onDuration;

	bool shouldStartPump(timeInMilliseconds currentTime)
	{
		return currentTime > waterPump.lastEndTime + offDuration;
	}

	bool shouldStopPump(timeInMilliseconds currentTime)
	{
		return currentTime > waterPump.lastStartTime + onDuration;
	}

public:
	StandaloneControlForWaterPump(WaterPump waterPump, timeInMilliseconds offDuration, timeInMilliseconds onDuration)
	{
		this->waterPump = waterPump;
		this->offDuration = offDuration;
		this->onDuration = onDuration;
	};

	void cycle(timeInMilliseconds currentTime)
	{
		if (waterPump.pumpIsOn)
		{
			if (shouldStopPump(currentTime))
			{
				waterPump.stop(currentTime);
			}
		}
		else
		{
			if (shouldStartPump(currentTime))
			{
				waterPump.start(currentTime);
			}
		}
	}
};
