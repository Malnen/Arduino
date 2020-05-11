#include "IRAlarm.h"
#include <iostream>
IRAlarm::IRAlarm()
{
    //ctor
}

IRAlarm::~IRAlarm()
{
    //dtor
}
void IRAlarm::print()
{
    for(int i = 0;i<sensors.size();i++){
        std::cout<<sensors[i]->getPin()<<std::endl;
    }
}
void IRAlarm::addSensor(IRSensor* sensor){
    sensors.push_back(sensor);
}
