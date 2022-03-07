#include <math.h>

const int redLEDPin = 16;	//GPIO16
const int greenLEDPin = 17; //GPIO17
const int blueLEDPin = 4;	//GPIO4

uint16_t redDutyCycle;
uint16_t greenDutyCycle;
uint16_t blueDutyCycle;

const int redPWMFreq = 5000;	//5kHz
const int redPWMChannel = 0;
const int redPWMResolution = 12;
const int RED_MAX_DUTY_CYCLE = (int)(pow(2, redPWMResolution) - 1);

const int bluePWMFreq = 8000;	//8kHz
const int bluePWMChannel = 0;
const int bluePWMResolution = 12;
const int BLUE_MAX_DUTY_CYCLE = (int)(pow(2, bluePWMResolution) - 1);

const int greenPWMFreq = 10000;	//10kHz
const int greenPWMChannel = 0;
const int greenPWMResolution = 12;
const int GREEN_MAX_DUTY_CYCLE = (int)(pow(2, greenPWMResolution) - 1);

const int ADC_RESOLUTION = 4095;	//12-bit


void setup() {
	Serial.begin(115200);

	//initialize pwm channels with frequencies and resolutions
	ledcSetup(redPWMChannel, redPWMFreq, redPWMResolution);
	ledcSetup(greenPWMChannel, greenPWMFreq, greenPWMResolution);
	ledcSetup(bluePWMChannel, bluePWMFreq, bluePWMResolution);

	//attach each GPIO pin to a PWM channel
	ledcAttachPin(redLEDPin, redPWMChannel);
	ledcAttachPin(blueLEDPin, bluePWMChannel);
	ledcAttachPin(greenLEDPin, greenLEDPin);
}


void loop() {
	//read analog input from three ADC inputs
	redDutyCycle = analogRead(A0);
	greenDutyCycle = analogRead(A3);
	blueDutyCycle = analogRead(A4);

	//map ADC output to macimum possible dutycycle
	redDutyCycle = map(redDutyCycle, 0, ADC_RESOLUTION, 0, RED_MAX_DUTY_CYCLE);
	greenDutyCycle = map(greenDutyCycle, 0, ADC_RESOLUTION, 0, GREEN_MAX_DUTY_CYCLE);
	blueDutyCycle = map(blueDutyCycle, 0, ADC_RESOLUTION, 0, BLUE_MAX_DUTY_CYCLE);

	Serial.println("RED -- Green -- Blue");
	Serial.print(redDutyCycle);
	Serial.print(" -- ");
	Serial.print(greenDutyCycle);
	Serial.print(" -- ");
	Serial.print(blueDutyCycle);
	Serial.print("\n");

	delay(1000);
}
