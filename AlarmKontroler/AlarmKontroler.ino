#include <IRAlarm.h>

IRAlarmWithTamper alarm(1);

int pinNotification = 3;
int pinAlarm = 2;
void setup() {
  alarm.addSensor(new IRSensorWithTamper(5,6));
  pinMode(pinAlarm, OUTPUT);
  pinMode(pinNotification, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
 // Serial.begin(9600);
}

void loop() {
  alarm.execute();
 
  if (digitalRead(pinNotification)) {
    if (alarm.alarm) {
      digitalWrite(pinAlarm, HIGH);
      digitalWrite(LED_BUILTIN, HIGH);
    }
  } else {
    alarm.alarm = false;
      digitalWrite(pinAlarm, LOW);
      digitalWrite(LED_BUILTIN, LOW);
  }
 //Serial.println("Alarm: "+ (String)alarm.alarm);
}
