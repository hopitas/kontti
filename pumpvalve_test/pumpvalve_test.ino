/*
 Name:		pumpvalve_test.ino
 Created:	10/10/2017 7:30:30 PM
 Author:	tero.toivola
*/

/* Antaa pinniin 13 jännitteen jaksollisesti */

int vent = 3; // valve pin

			   // setup routine:
void setup() {
	// define vent as output.
	Serial.begin(9600);
	pinMode(vent, OUTPUT);

}

// main program
void loop() {
	digitalWrite(vent, HIGH);     // valve on
	Serial.println("vent on");
	delay(5000);                 // wait 5s
	digitalWrite(vent, LOW);    // valve off
	Serial.println("vent off");
	delay(5000);               // wait 5s
}

