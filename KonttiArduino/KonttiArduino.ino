/*
 Name:		KonttiArduino.ino
 Created:	10/24/2017 8:07:16 PM
 Author:	tero.toivola
*/
#include <ArduinoJson.h>
#include <DHT_U.h>
#include <DHT.h>
#include "WaterPump.h"
#include "WaterLevelSensor.h"
#include "StandaloneControlForWaterPump.h"

#define DHTPIN 4											// DHT pin
#define PHPIN A0
#define DHTTYPE DHT22										// DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);									// Initialize DHT sensor for normal 16mhz Arduino

char messagechar[11];
int number = 0;
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
unsigned long previousMaxintervalMillis = 0;
unsigned long defaultwateringinterval = 21600000;  //1000 * 60 * 60 * 6	// Watering interval (milliseconds) 6 hours...
unsigned long defaultlighton = 57600000; //1000 * 60 * 60 * 16;					// lights on 16h / day
unsigned long defaultlightoff = 28800000; //1000 * 60 * 60 * 8;					// Lights off 8h / day
unsigned long maxinterval = 3600000; // if we don't get message from Rpi in maxinterval we go to default loop
unsigned long defaultdhtreadinginterval = 2000;						// Read temperature and humidity readings every 2 seconds.
float t1 = 0;
float h1 = 0;
float p1 = 0;
unsigned long previousDHTMillis = 0;
unsigned long defaultlightinterval;
unsigned long currentMillis;
unsigned long wateringStartTime = 0;
unsigned long maxwateringtime = 600000; // 1000 * 60 * 10
bool defaultlightswitch = true;
bool returnwatered = false;
bool lighton = true;
bool wlevelok = true;
bool watertime = false;
WaterPump waterPump = WaterPump(pump);
WaterLevelSensor waterLowerSensor = WaterLevelSensor(wLevelEmpty);
WaterLevelSensor waterHigherSensor = WaterLevelSensor(wLevelLimit);
timeInMilliseconds waterCycleDuration = 3600L * 8L * 1000L; // bigger than when directed by RasPi to avoid conflict
timeInMilliseconds waterOnDuration = 60L * 10L * 1000L;

StandaloneControlForWaterPump standaloneControlForWaterPump =
	StandaloneControlForWaterPump(&waterPump, &waterLowerSensor, &waterHigherSensor, waterCycleDuration, waterOnDuration);

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	while (!Serial) {
		// wait serial port initialization
	}
	dht.begin();
	pinMode(pump, OUTPUT);									//pump as output
	pinMode(light, OUTPUT);									//lights as output
	pinMode(wLevelLimit, INPUT);							//wlevellimit as input
	pinMode(wLevelEmpty, INPUT);							//wlevelempty as input
	digitalWrite(pump, LOW);								// pump off
	digitalWrite(light, HIGH);								// lights on
	defaultlightinterval = defaultlighton;					// start with lights on 16h
}

timeInMilliseconds lastReportMillis = 0L;
bool lastReportPumpState = false;
char report[250];

// the loop function runs over and over again until power down or reset
void loop() {
	currentMillis = millis();
	standaloneControlForWaterPump.cycle(currentMillis);
	readsensors(currentMillis, defaultdhtreadinginterval);

	//We start arduino default cycles only if there is no pulse in 1h from rpi
	pulseTimer(currentMillis, maxinterval);

  if (lastReportMillis + 1000 < currentMillis || (lastReportPumpState != waterPump.pumpIsOn))
  {
    lastReportMillis = currentMillis;
    lastReportPumpState = waterPump.pumpIsOn;
    snprintf(
		report,
		250,
		"{now: %lu, started: %lu, on: %d, reason: %d, hasWater: %d, overflow: %d}",
		currentMillis,
		waterPump.lastStartTime,
		waterPump.pumpIsOn ? 1 : 0,
		standaloneControlForWaterPump.lastStopReason + 1000,
		waterLowerSensor.HasWater() ? 1 : 0,
		waterHigherSensor.HasWater() ? 1 : 0
    );
  	Serial.println(report);
  }
}

void pulseTimer(unsigned long currentPulseMillis, unsigned long maxpulseinterval)
{
	// if we get pulse atleast every maxinterwall, we get the watering and light timers from RPI, else we start arduino  default intervals

	if (currentPulseMillis - previousMaxintervalMillis >= maxpulseinterval) {
		waterTimer(currentMillis, defaultwateringinterval);
		lightTimer(currentMillis, defaultlightinterval);
		// reset pulsetimer
		previousMaxintervalMillis = currentMillis;
	}
}

// Read temperature and humidity
void readsensors(unsigned long currentDHTMillis, unsigned long dhtreadinginterval) {

	if (currentDHTMillis - previousDHTMillis >= dhtreadinginterval) {
		h1 = dht.readHumidity();
		t1 = dht.readTemperature();
		p1 = readPH();
		previousDHTMillis = currentDHTMillis;
	}
}

//Go through watering cycle
void waterTimer(unsigned long currentWaterMillis, unsigned long wateringInterval) {

	if (currentWaterMillis - previousWaterMillis >= wateringInterval) {
		bool resetwatertimer =  watering(currentMillis, wateringStartTime);
		if (resetwatertimer == true)
		{
			previousWaterMillis = currentWaterMillis;
			watertime = false;
		}
		else
		{
			watertime = true;
		}
	}   // rise watered flag
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

float readPH()
{
	int measure = analogRead(PHPIN);
	double voltage = 5 / 1024.0 * measure; //classic digital to voltage conversion
	float Po = 7 + ((2.5 - voltage) / 0.18);

	return Po;
}

bool watering(unsigned long currentMillis, unsigned long wateringStartTime)
{
	bool watered;
	sensor_wLevelEmpty = digitalRead(wLevelEmpty);
	sensor_wLevelLimit = digitalRead(wLevelLimit);
	//check that lower container has water and higher container is not full
	if (sensor_wLevelEmpty == 0 && sensor_wLevelLimit == 1 && (currentMillis - wateringStartTime < maxwateringtime)) 
	{													// wlevel gives 1 when empty
		digitalWrite(pump, HIGH);  						// pump on
		watered = false;

	}
	else
	{
		if (sensor_wLevelEmpty == 1)
		{
			wlevelok = false;
		}
		else
		{
			wlevelok = true;
		}
		digitalWrite(pump, LOW);							// when waterlevel reached, or container empty, turn pump off
		returnwatered = true;
		watered = true;
		watertime = false;
	}
	
	return watered;
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

		json["temperature"] = t1;
		json["humidity"] = h1;
		json["ph"] = p1;
		json["lastWaterPumpStopReason"] = standaloneControlForWaterPump.lastStopReason;
		json["Watered"] = waterPump.pumpIsOn;
		json["wlevelok"] = wlevelok;


		switch (number)
		{
		case 1:
			// reset pulsetimer
			previousMaxintervalMillis = currentMillis;
			break;
		case 2:
			//set watering time flag
			standaloneControlForWaterPump.pleaseStartNow(currentMillis);
			watertime = true;
			break;
		case 3:
			//lights on
			json["Lightson"] = true;
			lightSwitch(true);
			break;
		case 4:
			//lights off
			json["Lightson"] = false;
			lightSwitch(false);
			break;
		case 5:
			//cooling??
			break;
		default:
			json["error"] = "Invalid request number";
			//Serial.println("No valid number");
			break;
		}
		json.printTo(Serial);
		json.remove("error"); // reported, now continue as normal
	}
}
