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
unsigned long previousDHTMillis = 0;
unsigned long defaultlightinterval;
unsigned long currentMillis;
bool defaultlightswitch = true;
bool returnwatered = false;
bool lighton = true;
bool wlevelok = true;
bool watertime = false;

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

// the loop function runs over and over again until power down or reset
void loop() {
	currentMillis = millis();
	readDHT(currentMillis, defaultdhtreadinginterval);

	if (watertime == true)
	{
	 watertime != watering(); // if we get watertime from rpi, it is true until watering is false.
	}

	//We start arduino default cycles only if there is no pulse in 1h from rpi
	pulseTimer(currentMillis, maxinterval);
}

void pulseTimer(unsigned long currentPulseMillis, unsigned long maxpulseinterval)
{
	// if we get pulse atleast every maxinterwall, we get the watering and light timers from RPI, else we start arduino  default intervals

	if (currentPulseMillis - previousMaxintervalMillis >= maxpulseinterval) {
		waterTimer(currentMillis, defaultwateringinterval);
		lightTimer(currentMillis, defaultlightinterval);
	}
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
		bool resetwatertimer =  watering();
		if (resetwatertimer == true)
		{
			previousWaterMillis = currentWaterMillis;
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

bool watering()
{
	bool watered;
	sensor_wLevelEmpty = digitalRead(wLevelEmpty);
	sensor_wLevelLimit = digitalRead(wLevelLimit);
	//check that lower container has water and higher container is not full
	if (sensor_wLevelEmpty == 0 && sensor_wLevelLimit == 1)
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

		switch (number)
		{
		case 1:
			json["temperature"] = t1;
			json["humidity"] = h1;
			json["watered"] = returnwatered;					// If watered between serial events, send true to Rpi and then set watered flag false
			json["wlevelok"] = wlevelok;
			json["lighton"] = lighton;					// Tells if light is on or off					
			json.printTo(Serial);
			if (returnwatered == true)						// We want to make sure we send atleast 1 watered flag when its done and only then set it false
			{
				returnwatered = false;
			}
			// reset pulsetimer
			previousMaxintervalMillis = currentMillis;
			break;
		case 2:
			//set watering time flag
			watertime = true;
			break;
		case 3:
			//lights on
			lightSwitch(true);
			break;
		case 4:
			//lights off
			lightSwitch(false);
			break;
		case 5:
			//cooling??
			break;
		default:
			//Serial.println("No valid number");
			break;
		}
	}
}