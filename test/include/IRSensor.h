#ifndef IRSENSOR_H
#define IRSENSOR_H

namespace InfraRedSensor
{
class IRSensor
{
public:
    IRSensor();
    IRSensor(int pin);
    int getPin();
    virtual ~IRSensor();
    bool enabled = true;
protected:

private:
    int pin = 0;
};
}
#endif // IRSENSOR_H

