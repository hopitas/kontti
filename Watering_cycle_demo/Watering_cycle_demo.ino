/* Watering cycle demo*/

int pump = 3;												// pump pin
byte wLevelLimit = 6;										// Upper container max water level 
byte wLevelEmpty = 5;										// lower container empty
short sensor_wLevelLimit = 1;								// Upper container water level sensor reading
short sensor_wLevelEmpty = 0;								// Lower container wlevel
unsigned long previousMillis = 0;
long defaultwateringinterval = 1000 * 10;					// interval at which to blink (milliseconds) 5 minute...

// setup routine:
void setup() {
	Serial.begin(9600);
	pinMode(pump, OUTPUT);
	//pump as output
	pinMode(wLevelLimit, INPUT);							//wlevellimit as input
	pinMode(wLevelEmpty, INPUT);							//wlevelempty as input
	digitalWrite(pump, LOW);								// pump off
}

// main program
void loop() {
	unsigned long currentMillis = millis();
	watering(currentMillis, defaultwateringinterval);
}

void watering(unsigned long currentMillis, long wateringInterval) {

	if (currentMillis - previousMillis >= wateringInterval) {
			sensor_wLevelEmpty = digitalRead(wLevelEmpty);
			sensor_wLevelLimit = digitalRead(wLevelLimit);

		while (sensor_wLevelEmpty == 0 && sensor_wLevelLimit == 1) //check that lower container has water and higher container is not full
		{															// wlevel gives 1 when empty
			digitalWrite(pump, HIGH);  	 // pump on
			delay(5000);                 // wait 5s
			sensor_wLevelEmpty = digitalRead(wLevelEmpty);
			sensor_wLevelLimit = digitalRead(wLevelLimit);
		}
		digitalWrite(pump, LOW);		// when waterlevel reached, or container empty, turn off pump
		// save the last time when watered
		previousMillis = currentMillis;
	}
}