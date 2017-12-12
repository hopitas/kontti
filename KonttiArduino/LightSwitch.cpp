// 
// 
// 

#include "LightSwitch.h"

int _light = 0;

void LightSwitch::Toggle(bool on) {
	digitalWrite(_light, on ? HIGH : LOW);
};
