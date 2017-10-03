
//define analog inputs to which we have connected our sensors
byte waterLevel = 3; // Water level sensor pin

short sensor_val = 0;    // water level sensor reading

void setup() {
	//open serial port
	pinMode(waterLevel, INPUT); // define as input
	Serial.begin(9600);
}



void loop() {
	// read the value from the moisture-sensing probes, print it to screen, and wait a second
	sensor_val = digitalRead(waterLevel); // read value from diode and print it
	Serial.print("waterlevel reads ");
	Serial.println(sensor_val);
	delay(1000); // wait 1s

}
