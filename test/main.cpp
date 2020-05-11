#include <iostream>
#include <IRSensor.h>
#include <IRAlarm.h>
using namespace std;
using namespace InfraRedSensor;
int main()
{
    IRAlarm *alarm = new IRAlarm();
    alarm->addSensor(new IRSensor(2));
    alarm->addSensor(new IRSensor(3));
    alarm->addSensor(new IRSensor(4));
    alarm->addSensor(new IRSensor(5));
    alarm->addSensor(new IRSensor(1));
    alarm->print();
    return 0;
}
