#ifndef IRALARM_H
#define IRALARM_H
#include <IRSensor.h>
class IRAlarm
{

    public:
        IRAlarm();
        IRAlarm(int size);

		void changeSensorStatus(int i,bool status);
		virtual void addSensor(IRSensor *sensor);
		virtual void execute();
		bool alarm = false;
    protected:
	

    private:
		IRSensor *sensors;	
		int next = 0;
		int size = 0;
};

class IRAlarmWithTamper: public IRAlarm
{

    public:
        IRAlarmWithTamper();
        IRAlarmWithTamper(int size);
		void execute(); 
		void addSensor(IRSensorWithTamper *sensor);
    protected:

    private:
		IRSensorWithTamper *sensors;	
		int next = 0;
		int size = 0;
};

#endif // IRALARM_H
