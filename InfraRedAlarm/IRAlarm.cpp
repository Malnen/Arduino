#include "IRAlarm.h"
#include <IRSensor.h>
#include "Arduino.h"
IRAlarm::IRAlarm()
{
	
}
IRAlarm::IRAlarm(int size)
{
	this->size = size;
	this->sensors = new IRSensor[size];
}
void IRAlarm::changeSensorStatus(int i,bool status)
{
	(sensors + i)->enabled = status;
}	
void IRAlarm::addSensor(IRSensor *sensor)
{
	if(next<size)
	{
		*(sensors + next) = *sensor;
		next++;
	}
}
void IRAlarm::execute()
{
	for(int i = 0;i<next;i++){
		if((sensors + i)->enabled)
		{
			if(!digitalRead((sensors + i)->getPin()))
			{
				alarm = true;
			}
				//Serial.println("Sensor pin " + (String)(sensors + i)->getPin() + ": " + digitalRead((sensors + i)->getPin()));
		}
	}
}


IRAlarmWithTamper::IRAlarmWithTamper() : IRAlarm()
{
	
}
IRAlarmWithTamper::IRAlarmWithTamper(int size) : IRAlarm(size)
{
	this->size = size;
	this->sensors = new IRSensorWithTamper[size];
}

void IRAlarmWithTamper::execute()
{
	for(int i = 0;i<next;i++){
		if((sensors + i)->enabled)
		{
			if(!digitalRead((sensors + i)->getPin()) || !digitalRead((sensors + i)->getTamperPin()))
			{
				alarm = true;
			}
				//Serial.println("Sensor pin " + (String)(sensors + i)->getPin() + ": " + digitalRead((sensors + i)->getPin()));
				//Serial.println("Sensor tamper pin " + (String)(sensors + i)->getTamperPin() + ": " + digitalRead((sensors + i)->getTamperPin()));
		}
	}
}
void IRAlarmWithTamper::addSensor(IRSensorWithTamper*sensor)
{
	if(next<size)
	{
		*(sensors + next) = *sensor;
		next++;
	}
}