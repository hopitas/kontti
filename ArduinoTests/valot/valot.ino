#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// constants won't change. They're used here to set pin numbers:
const int buttonPin = D1;     // the number of the pushbutton pin
const int ledPin =  D2;      // the number of the LED pin
char ssid[] = "Leasegreen WLAN";
char pass[] = "Tammikuu2017!";

const int httpsPort = 443;
const char* host = "prod-05.westeurope.logic.azure.com";

  String url = "/workflows/c38a4358bc4a4d37a1ef45c69433d64e/triggers/manual/paths/invoke?api-version=2016-10-01&sp=%2Ftriggers%2Fmanual%2Frun&sv=1.0&sig=4qzws7mOm-xLkREw6i4a-CjIpJE8nkNNziZ9mcuaRnE";
 

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
// initialize the LED pin as an output:
// initialize the pushbutton pin as an input:

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // Connect to Wi-Fi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to...");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wi-Fi connected successfully");


  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

}

void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
    digitalWrite(ledPin, HIGH);

httpsend();
  
    // turn LED off:
    digitalWrite(ledPin, LOW);
  }
}

void httpsend()
{
 if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
 
   HTTPClient http;    //Declare object of class HTTPClient
 
   http.begin("https://prod-05.westeurope.logic.azure.com:443/workflows/c38a4358bc4a4d37a1ef45c69433d64e/triggers/manual/paths/invoke?api-version=2016-10-01&sp=%2Ftriggers%2Fmanual%2Frun&sv=1.0&sig=4qzws7mOm-xLkREw6i4a-CjIpJE8nkNNziZ9mcuaRnE");      //Specify request destination
   http.addHeader("Content-Type", "text/plain");  //Specify content-type header
 
   int httpCode = http.POST("10");   //Send the request
   String payload = http.getString();                  //Get the response payload
 
   Serial.println(httpCode);   //Print HTTP return code
   Serial.println(payload);    //Print request response payload
 
   http.end();  //Close connection
 
 }else{
 
    Serial.println("Error in WiFi connection");   
 
 }
}

