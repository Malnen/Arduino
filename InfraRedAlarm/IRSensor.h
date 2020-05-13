#ifndef IRSENSOR_H
#define IRSENSOR_H
class IRSensor
{
public:
    IRSensor();
    IRSensor(int pin);
	bool enabled = true;
	int getPin();
protected:
	
private:
	int pin = 0;
};
#endif // IRSENSOR_H

