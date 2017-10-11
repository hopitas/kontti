/* Watering cycle demo*/

byte valve = 2;												// valve pin
byte pump = 3;												// pump pin
byte wLevelLimit = 6;										// Upper container max water level 
byte wLevelEmpty = 5;										// lower container empty
short sensor_wLevelLimit = 0;								// Upper container water level sensor reading
short sensor_wLevelEmpty = 0;								// Lower container wlevel
unsigned long previousMillis = 0;
long defaultwateringinterval = 1000 * 60 * 5;				// interval at which to blink (milliseconds) 5 minute...

// setup routine:
void setup() {
	Serial.begin(9600);
	pinMode(valve, OUTPUT);									//valve as output
	pinMode(pump, OUTPUT);									//pump as output
	pinMode(wLevelLimit, INPUT);							//wlevellimit as input
	pinMode(wLevelEmpty, INPUT);							//wlevelempty as input
	digitalWrite(valve, HIGH);								// valve on
	digitalWrite(pump, LOW);								// pump off
}

// main program
void loop() {
	unsigned long currentMillis = millis();
	watering(currentMillis, defaultwateringinterval);
}

void watering(unsigned long currentMillis, long wateringInterval) {

	if (currentMillis - previousMillis >= wateringInterval) {
		while (sensor_wLevelEmpty == 0 && sensor_wLevelLimit == 1) //check that lower container has water and higher container is not full
		{															// wlevel gives 1 when empty
			digitalWrite(valve, LOW);						//close the valve
			digitalWrite(pump, HIGH);						// pump on
			sensor_wLevelEmpty = digitalRead(wLevelEmpty);
			sensor_wLevelLimit = digitalRead(wLevelLimit);
		}
		digitalWrite(pump, LOW);							// when waterlevel reached, or container empty, turn off pump
		digitalWrite(valve, HIGH);							// open valve
		// save the last time when watered
		previousMillis = currentMillis;
	}
}