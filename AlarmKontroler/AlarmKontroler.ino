#include <IRAlarm.h>

IRAlarm alarm(1);

int pinNotification = 3;
int pinAlarm = 2;
void setup() {
  alarm.addSensor(new IRSensor(4));
  alarm.addSensor(new IRSensor(5));
  alarm.addSensor(new IRSensor(6));
  pinMode(pinAlarm, OUTPUT);
  pinMode(pinNotification, INPUT);
  //Serial.begin(9600);

}

void loop() {
  alarm.execute();

  if (digitalRead(pinNotification)) {
    if (alarm.alarm) {
      digitalWrite(pinAlarm, HIGH);
    }
  } else {
    alarm.alarm = false;
      digitalWrite(pinAlarm, LOW);
  }
 // Serial.println("Alarm: "+ (String)alarm.alarm);
 // delay(1000);
}
