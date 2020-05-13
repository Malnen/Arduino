#include "IRAlarm.h"
#include <IRSensor.h>
#include "Arduino.h"
IRAlarm::IRAlarm()
{
    //ctor
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