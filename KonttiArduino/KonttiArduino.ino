/*
 Name:		KonttiArduino.ino
 Created:	10/24/2017 8:07:16 PM
 Author:	tero.toivola
*/
#include <ArduinoJson.h>
#include <DHT_U.h>
#include <DHT.h>

#define DHTPIN 4											// DHT pin
#define DHTTYPE DHT22										// DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);									// Initialize DHT sensor for normal 16mhz Arduino

String sensor1;
char messagechar[11];
int number = 0;
float t1 = 0;
float h1 = 0;
int readamount = 0;
long randNumber;
StaticJsonBuffer<200> jsonBuffer;
JsonObject& json = jsonBuffer.createObject();
int pump = 3;												// pump pin
int light = 4;												// light pin
byte wLevelLimit = 6;										// Upper container max water level 
byte wLevelEmpty = 5;										// lower container empty
short sensor_wLevelLimit = 1;								// Upper container water level sensor reading
short sensor_wLevelEmpty = 0;								// Lower container wlevel
unsigned long previousWaterMillis = 0;
unsigned long previousLightMillis = 0;
unsigned long previousDHTMillis = 0;
long defaultwateringinterval = 1000 * 60 * 60 * 6;			// Watering interval (milliseconds) 6 hours...
long defaultdhtreadinginterval = 2000;						// Read temperature and humidity readings every 2 seconds.
long defaultlighton = 1000 * 60 * 60 * 16;					// lights on 16h / day
long defaultlightoff = 1000 * 60 * 60 * 8;					// Lights off 8h / day
long defaultlightinterval;
bool defaultlightswitch = true;

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	while (!Serial) {
		// wait serial port initialization
	}
	dht.begin();
	randomSeed(analogRead(0));
	pinMode(pump, OUTPUT);									//pump as output
	pinMode(light, OUTPUT);									//lights as output
	pinMode(wLevelLimit, INPUT);							//wlevellimit as input
	pinMode(wLevelEmpty, INPUT);							//wlevelempty as input
	digitalWrite(pump, LOW);								// pump off
	digitalWrite(light, HIGH);								// lights on
	defaultlightinterval = defaultlighton;					// start with lights on 16h
}

// the loop function runs over and over again until power down or reset
void loop() {
	unsigned long currentMillis = millis();
	watering(currentMillis, defaultwateringinterval);
	readDHT(currentMillis, defaultdhtreadinginterval);
	lightTimer(currentMillis, defaultlightinterval);
}


//Lightning default timer
void lightTimer(unsigned long currentLightMillis, long lightInterval)
{
	if (currentLightMillis - previousLightMillis >= lightInterval) {
		defaultlightswitch = !defaultlightswitch;
		lightSwitch(defaultlightswitch); //switch lights
		previousLightMillis = currentLightMillis;

		// switch between on/off interval
		if (defaultlightinterval == defaultlighton)
		{
			defaultlightinterval = defaultlightoff;
		}
		else
		{
			defaultlightinterval = defaultlighton;
		}
	}
}

void lightSwitch(bool on)
{
	if (on == true)
	{
		digitalWrite(light, HIGH);
	}
	else
	{
		digitalWrite(light, LOW);
	}
}

//Go through watering cycle
void watering(unsigned long currentWaterMillis, long wateringInterval) {

	if (currentWaterMillis - previousWaterMillis >= wateringInterval) {
		sensor_wLevelEmpty = digitalRead(wLevelEmpty);
		sensor_wLevelLimit = digitalRead(wLevelLimit);

		//check that lower container has water and higher container is not full
		while (sensor_wLevelEmpty == 0 && sensor_wLevelLimit == 1)
		{													// wlevel gives 1 when empty
			digitalWrite(pump, HIGH);  						// pump on
			delay(5000);									// wait 5s
			sensor_wLevelEmpty = digitalRead(wLevelEmpty);
			sensor_wLevelLimit = digitalRead(wLevelLimit);
		}
		digitalWrite(pump, LOW);							// when waterlevel reached, or container empty, turn off pump
															// save the last time when watered
		previousWaterMillis = currentWaterMillis;
	}
}

// Read temperature and humidity
void readDHT(unsigned long currentDHTMillis, long dhtreadinginterval) {

	if (currentDHTMillis - previousDHTMillis >= dhtreadinginterval) {
		h1 += dht.readHumidity();
		t1 += dht.readTemperature();
		readamount++;
		previousDHTMillis = currentDHTMillis;
	}
}

// callback to RPI for received data
void serialEvent()
{
	while (Serial.available())
	{
		number = Serial.read();

		switch (number)
		{
		case 1:
			// Take average
			t1 = t1 / readamount;
			h1 = h1 / readamount;
			readamount = 0;
			json["temperature"] = t1;
			json["humidity"] = h1;
			json.printTo(Serial);
			break;

		default:
			//Serial.println("No valid number");
			break;
		}
	}
}