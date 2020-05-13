#ifndef IRALARM_H
#define IRALARM_H
#include <IRSensor.h>
class IRAlarm
{

    public:
        IRAlarm();
        IRAlarm(int size);

		void changeSensorStatus(int i,bool status);
		void addSensor(IRSensor *sensor);
		void execute();
		bool alarm = false;
    protected:

    private:
	int next = 0;
	int size = 0;
	IRSensor *sensors;
};

#endif // IRALARM_H
