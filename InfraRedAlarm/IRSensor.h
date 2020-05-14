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

class IRSensorWithTamper : public IRSensor
{
public:
    IRSensorWithTamper();
    IRSensorWithTamper(int pin, int tamperPin);
	bool enabled = true;
	int getTamperPin();
protected:
	
private:
	int tamperPin = 0;
};
#endif // IRSENSOR_H

