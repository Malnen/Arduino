#include "IRSensor.h"
#include "Arduino.h"
IRSensor::IRSensor()
{
	
}
IRSensor::IRSensor(int pin)
{
	this->pin = pin;
	pinMode(pin,INPUT);	
}
int IRSensor::getPin()
{
	return this->pin;
}

IRSensorWithTamper::IRSensorWithTamper(): IRSensor()
{
	
}
IRSensorWithTamper::IRSensorWithTamper(int pin, int tamperPin): IRSensor(pin)
{
	this->tamperPin = tamperPin;
	pinMode(tamperPin,INPUT);	
}
int IRSensorWithTamper::getTamperPin()
{
	return this->tamperPin;
}
