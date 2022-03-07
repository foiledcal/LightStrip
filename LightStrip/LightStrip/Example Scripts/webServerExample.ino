#include <WiFi.h>

#define gpio16LEDPin 16
#define gpio17LEDPin 17

const char* ssid = "fantastic_four";
const char * password = "cougarcub"

int gpio16value;
int gpio17value;

WiFiServer server(80);	//instance of WiFiServer with port number 80, the default port for HTTP Web Server

// a string to capture the incoming HTTP GET request
String request;


void Setup() {
	Serial.begin(115200);

	//configure GPIO16 and GPIO17 pins as outputs
	pinMode(gpio16LEDPin, OUTPUT);
	pinMode(gpio17LEDPin, OUTPUT);

	//set initial values of LED pins as LOW
	digitalWrite(gpio16LEDPin, LOW);
	digitalWrite(gpio17LEDPin, LOW);

	Serial.print
}

https://www.electronicshub.org/esp32-web-server/