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


