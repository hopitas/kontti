// LightSwitch.h

#ifndef _LIGHTSWITCH_h
#define _LIGHTSWITCH_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

class LightSwitch
{
public:
	void Toggle(bool on);
};

#endif

