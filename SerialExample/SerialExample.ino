String sensor1;
char messagechar[11];
int number = 0;
float t1 = 0;
float h1 = 0;
long randNumber;

void setup() {

	Serial.begin(115200);
	randomSeed(analogRead(0));
	delay(2000);
}

void loop() {

	t1 = (float)random(1000, 9000) / 100;
	h1 = (float)random(1000, 9000) / 100;
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
			sensor1 = String(t1, 2) + "/" + String(h1, 2);
			Serial.print(sensor1);
			break;

		default:
			//Serial.println("No valid number");
			break;
		}
	}
}