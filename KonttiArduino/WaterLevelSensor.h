

#ifndef _WATERLEVELSENSOR_h
#define _WATERLEVELSENSOR_h

#include "Arduino.h"

typedef unsigned long timeInMilliseconds;

class WaterLevelSensor
{
private:
	int pin_number;
	const int senses_water = 0;

public:
	WaterLevelSensor(int pin_number)
	{
		this->pin_number = pin_number;
	};

	bool HasWater();

};

#endif
