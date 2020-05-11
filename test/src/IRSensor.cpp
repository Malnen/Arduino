#include "IRSensor.h"

namespace InfraRedSensor
{
IRSensor::IRSensor(int pin)
{
    this->pin = pin;
}
IRSensor::IRSensor()
{

}
int IRSensor::getPin()
{
    return this->pin;
}

IRSensor::~IRSensor()
{
    //dtor
}
}
