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
long randNumber;
StaticJsonBuffer<200> jsonBuffer;
JsonObject& json = jsonBuffer.createObject();
int pump = 3;												// pump pin
int light = 2;												// light pin
byte wLevelLimit = 6;										// Upper container max water level 
byte wLevelEmpty = 5;										// lower container empty
short sensor_wLevelLimit = 1;								// Upper container water level sensor reading
short sensor_wLevelEmpty = 0;								// Lower container wlevel
unsigned long previousWaterMillis = 0;
unsigned long previousLightMillis = 0;
unsigned long previousDHTMillis = 0;
unsigned long defaultwateringinterval = 21600000;  //1000 * 60 * 60 * 6	// Watering interval (milliseconds) 6 hours...
unsigned long defaultdhtreadinginterval = 2000;						// Read temperature and humidity readings every 2 seconds.
unsigned long defaultlighton = 57600000; //1000 * 60 * 60 * 16;					// lights on 16h / day
unsigned long defaultlightoff = 28800000; //1000 * 60 * 60 * 8;					// Lights off 8h / day
unsigned long defaultlightinterval;
bool defaultlightswitch = true;
bool watered = true;
bool lighton = true;

class ReadDHT
{

};


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
	waterTimer(currentMillis, defaultwateringinterval);
	readDHT(currentMillis, defaultdhtreadinginterval);
	lightTimer(currentMillis, defaultlightinterval);
}

// Read temperature and humidity
void readDHT(unsigned long currentDHTMillis, unsigned long dhtreadinginterval) {

	if (currentDHTMillis - previousDHTMillis >= dhtreadinginterval) {
		h1 = dht.readHumidity();
		t1 = dht.readTemperature();
		previousDHTMillis = currentDHTMillis;
	}
}


//Go through watering cycle
void waterTimer(unsigned long currentWaterMillis, unsigned long wateringInterval) {

	if (currentWaterMillis - previousWaterMillis >= wateringInterval) {

		sensor_wLevelEmpty = digitalRead(wLevelEmpty);
		sensor_wLevelLimit = digitalRead(wLevelLimit);
		//check that lower container has water and higher container is not full
		if (sensor_wLevelEmpty == 0 && sensor_wLevelLimit == 1)
		{													// wlevel gives 1 when empty
			digitalWrite(pump, HIGH);  						// pump on
		}
		else
		{
			digitalWrite(pump, LOW);							// when waterlevel reached, or container empty, turn pump off
			watered = true;
			previousWaterMillis = currentWaterMillis;
		}   // rise watered flag
	}
}

//Lightning default timer
void lightTimer(unsigned long currentLightMillis, unsigned long lightInterval)
{
	if (currentLightMillis - previousLightMillis >= lightInterval) {

		// switch between on/off interval
		if (defaultlightinterval == defaultlighton)
		{
			defaultlightinterval = defaultlightoff;
			defaultlightswitch = false;
		}
		else
		{
			defaultlightinterval = defaultlighton;
			defaultlightswitch = true;
		}
		lightSwitch(defaultlightswitch); //switch lights
		previousLightMillis = currentLightMillis;
	}
}

void lightSwitch(bool on)
{
	if (on == true)
	{
		digitalWrite(light, HIGH);
		lighton = true;
	}
	else
	{
		digitalWrite(light, LOW);
		lighton = false;
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
			json["temperature"] = t1;
			json["humidity"] = h1;
			json["watered"] = watered;					// If watered between serial events, send true to Rpi and then set watered flag false
			json["lighton"] = lighton;					// Tells if light is on or off					
			json.printTo(Serial);
			if (watered == true)						// We want to make sure we send atleast 1 watered flag when its done and only then set it false
			{
				watered = false;
			}
			break;

		default:
			//Serial.println("No valid number");
			break;
		}
	}
}