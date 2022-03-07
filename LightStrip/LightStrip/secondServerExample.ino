#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

int ledPin = 12;

String ledState;

const char* ssid = "fantastic_four";
const char* password = "cougarcub";
//create asyncwevserver object on port 80
AsyncWebServer server(80);

//replaces placeholder with led state value
String processor(const String& var) {
	Serial.println(var);
	if (var == "STATE") {
		if (digitalRead(ledPin)) {
			ledState = "ON";
		}
		else {
			ledState = "OFF";
		}
		Serial.print(ledState);
		return ledState;
	}
	return String();
}

void setup() {
	Serial.begin(115200);
	pinMode(ledPin, OUTPUT);

	//initialize SPIFFS
	if (!SPIFFS.begin(true)) {
		Serial.println("An error occured wile mounting SPIFFS");
		return;
	}

	//connect to wifi
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
		delay(100);
		Serial.println("COnnecting to WiFI");
	}

	//display local IP
	Serial.println(WiFi.localIP());

	//route for root/web page
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});

	//route to load style.css file
	server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(SPIFFS, "/style.css", "text/css");
	});

	//route to set gpio to high
	server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
		digitalWrite(ledPin, HIGH);
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});
		
	//route to set gpio to low
	server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
		digitalWrite(ledPin, LOW);
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});

	//start server
	server.begin();
}

void loop() {

}