//general libraries
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
//WiFi libraries
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
//OLED libraries
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

//Sliders
#define CCTpin A2			//cannot use A0, A1, or A2 with WiFi running
#define brightnessPin A3
#define ADCres 12			//12-bit ADC resolution is max

int CCTvalue = 0;
int brightnessValue = 0;

//OLED
Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);
#define BUTTON_A 15
#define BUTTON_B 32
#define BUTTON_C 14

//PWM
#define coolPin 12
#define warmPin 33
#define coolFreq 1000	//1 kHz
#define coolChannel 2
#define coolRes 16		//16-bit
#define coolMaxDutyCycle (int)(pow(2, coolRes) - 1)
uint16_t coolDutyCycle = 0;		//really big number to hold duty cycle
#define warmFreq 1000	//1 kHz
#define warmChannel 4
#define warmRes 16		//16-bit
#define warmMaxDutyCycle (int)(pow(2, warmRes) - 1)
uint16_t warmDutyCycle = 0;		//really big number to hold duty cycle

//Server
const char* ssid = "fantastic_four";
const char* password = "cougarcub";
AsyncWebServer server(80);	//instance of wifi server with port number 80
String request; //a string to capture the incoming HTTP GET request
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
int botherWithServer = 1;


//Loop
int mode = 0;	//0: const, 1: simulate
int screenUpdateDelay = 100;
int lastUpdate = millis();


void setup() {

	Serial.begin(115200);

	//Sliders
	pinMode(CCTpin, INPUT);
	pinMode(brightnessPin, INPUT);

	//OLED
	pinMode(BUTTON_A, INPUT_PULLUP);
	pinMode(BUTTON_B, INPUT_PULLUP);
	pinMode(BUTTON_C, INPUT_PULLUP);
	delay(250);	//wait for display to power on
	display.begin(0x3C, true);	//address 0x3C default
	display.clearDisplay();		//clear the buffer
	display.display();			//display nothing
	display.setRotation(1);
	display.setTextSize(1);
	display.setTextColor(SH110X_WHITE);
	display.setCursor(0, 0);

	//PWM
		//initialize pwm channels with frequencies and resolutions
	ledcSetup(coolChannel, coolFreq, coolRes);
	ledcSetup(warmChannel, warmFreq, warmRes);
		//attach each GPIO pin to a PWM channel
	ledcAttachPin(coolPin, coolChannel);
	ledcAttachPin(warmPin, warmChannel);

	//server
	if (botherWithServer) {
		//initialize SPIFFS
		if (!SPIFFS.begin(true)) {
			display.println("Error occured while ");
			display.print("mounting SPIFFS");
			display.display();
			return;
		}

		//Connect to network
		WiFi.begin(ssid, password);
		display.println("Connecting to: ");
		display.println(ssid);
		display.display();

		unsigned long startTime = millis();
		int interval = 200;		//milliseconds between asterisks
		int charLimit = 10;
		int currentChar = 0;

		//
		while (WiFi.status() != WL_CONNECTED) {
			if (millis() - startTime >= interval) {
				if (currentChar < charLimit) {
					display.print("*");
				}
				else {
					display.println("*");
					currentChar = 0;
				}
				display.display();
				currentChar += 1;
				startTime = millis();
			}
			if (millis() - startTime >= 10000) {	//if connection takes over 10s
				display.clearDisplay();
				display.println("Failed to ");
				display.println("connect in");
				display.println("10 seconds");
				display.display();
				delay(60000);
			}
		}
		if (WiFi.status() == WL_CONNECTED) {
			display.clearDisplay();
			display.println("Connected ");
			display.println("to: ");
			display.print(WiFi.SSID());
			display.display();
			delay(1000);
		}

		//route for root/web page
		server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
			request->send(SPIFFS, "/index.html", String(), false, processor);
		});

		//route to load style.css file
		server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
			request->send(SPIFFS, "/style.css", "text/css");
		});

		/*
		routes for IO control here
		*/

		//start HTTP web server
		display.println("Starting web server");
		server.begin();
		display.println("Web server started.");
		display.println("URL is: ");
		display.print("http://");
		display.println(WiFi.localIP());
	}

}


void loop() {

	WiFiClient client = server.available();	//checks if a client is available
	if (client) {
		
	}


	CCTvalue = analogRead(CCTpin);
	brightnessValue = analogRead(brightnessPin);

	if (mode == 0) {
		warmDutyCycle = brightnessValue * 16;
		coolDutyCycle = brightnessValue * 16;
	}

	//ledcWrite(coolChannel, coolDutyCycle - 1);
	ledcWrite(warmChannel, warmDutyCycle);
	ledcWrite(coolChannel, coolDutyCycle);

	//update display
	if (millis() - lastUpdate >= screenUpdateDelay) {
		display.clearDisplay();
		display.setCursor(0, 0);
		display.print("Cool max duty cycle: ");
		display.println(coolMaxDutyCycle);
		display.print("Warm max duty cycle: ");
		display.println(warmMaxDutyCycle);
		display.print("Cool: ");
		display.println(coolDutyCycle);
		display.print("Warm: ");
		display.println(warmDutyCycle);
		display.print("CCT val: ");
		display.println(CCTvalue);
		display.print("Brightness: ");
		display.println(brightnessValue);
		display.display();
		lastUpdate = millis();
	}

}