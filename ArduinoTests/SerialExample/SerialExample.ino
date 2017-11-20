#include <ArduinoJson.h>
#include <DHT_U.h>
#include <DHT.h>

#define DHTPIN 4     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

String sensor1;
char messagechar[11];
int number = 0;
float t1 = 0;
float h1 = 0;
long randNumber;
StaticJsonBuffer<200> jsonBuffer;
JsonObject& json = jsonBuffer.createObject();



void setup() {

	Serial.begin(115200);
	while (!Serial) {
		// wait serial port initialization
	}

	dht.begin();
	randomSeed(analogRead(0));
}

void loop() {
	h1 = dht.readHumidity();
	t1 = dht.readTemperature();
	/*
	Serial.print("Temperature: ");
	Serial.println(t1);
	Serial.print("Humidity: ");
	Serial.println(h1);
	*/
	/*
	json["temperature"] = t1;
	json["humidity"] = h1;
	json.printTo(Serial);
	Serial.print("\n");
	*/
	delay(2000);
}

void sendData() {
	Serial.write(messagechar);
}

// callback for received data
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

			json.printTo(Serial);
			break;

		default:
			//Serial.println("No valid number");
			break;
		}
	}
}