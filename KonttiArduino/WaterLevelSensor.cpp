
#include "WaterLevelSensor.h"

	bool WaterLevelSensor::HasWater()
	{
		return digitalRead(pin_number) == senses_water;
	}
