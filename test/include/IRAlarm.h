#ifndef IRALARM_H
#define IRALARM_H
#include <IRSensor.h>
#include <vector>
using namespace InfraRedSensor;
using namespace std;
class IRAlarm
{
    public:
        IRAlarm();
        virtual ~IRAlarm();
        void print();
        void addSensor(IRSensor* sensor);
    protected:

    private:
        std::vector<IRSensor*> sensors;
};

#endif // IRALARM_H
