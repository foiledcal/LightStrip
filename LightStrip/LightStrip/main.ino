/*
 Name:		LightStrip.ino
 Created:	2/23/2022 7:18:30 PM
 Author:	Cal Merriman

 Board: Adafruit ESP32 Feather
 */

//test line edit

/*shopping list:
Waveform
1x FilmGrade HYBRID LED Strip Light 24V
1x FilmGrade DC Power Supply for LED Strip 24V
2x Aluminum Channel for LED Flex Strip - 5 PACK
*/

/* NOTES
do not use pwm channels 0 and 1, they interfere. channels 2 and 4 are tested.
waveform's dimmer is 16-bit, at least 480Hz

*/

/* IDEAS
when turning on, give brightness an s-curve
sleep display after a minute
verify time with source
to smooth slider shakiness, only begin fading once input changes by a certain percent
*/

/* BUTTON UI


*/

/* The Plan

inputs:
	slider: color temp
	slider: brightness
	button: switch modes
	button: on/off

outputs:
	two PWM channels (one per color), two PWM signals per channel (two strips, two power supplies, two mosfets all per color channel
	128x64 OLED

run modes:
	simulate daylight
	alarm (accelerated sunrise at preset time, then simulate daylight)
	constant
	

eventually have web app
	leave gpio pins 21 (RX2) and 22 (TX2) free for this
*/


/*Outline

setup()
serial.begin
oled
button digital inputs
slider analog inputs
pwm outputs

loop()
determine mode()
	if sliders move
		set to constant
	if mode button has a new press
		toggle mode

if constant
	read sliders
	compare to old light state
	if new state different from old
		fade
if simulating
	determine needed cct
	fade

*/


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
#define CCTpin A0
#define brightnessPin A1
#define ADCres 12			//12-bit ADC resolution

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
uint16_t coolDutyCycle;
#define warmFreq 1000	//1 kHz
#define warmChannel 4
#define warmRes 16		//16-bit
#define warmMaxDutyCycle (int)(pow(2, warmRes) - 1)
uint16_t warmDutyCycle;

//Server
const char* ssid = "fantastic_four";
const char* password = "cougarcub";
WiFiServer server(80);	//instance of wifi server with port number 80
String request; //a string to capture the incoming HTTP GET request
int botherWithServer = 1;



void setup() {
	
	Serial.begin(115200);

	//Sliders
	pinMode(CCTpin, INPUT);
	pinMode(brightnessPin, INPUT);

	//OLED
	pinMode(BUTTON_A, INPUT_PULLUP);
	pinMode(BUTTON_B, INPUT_PULLUP);
	pinMode(BUTTON_C, INPUT_PULLUP);

	delay(250);
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
}


void loop() {

}