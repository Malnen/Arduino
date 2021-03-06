#include <SPI.h>
#include <Ethernet.h>
#include <Time.h>
#include <EthernetUdp.h>
#include <NTPClient.h>
#include <BlynkSimpleEthernet.h>

bool alarm = false;
bool wlacznik = true;
bool swiatlo = false;
bool pastuch = false;
bool toggleLightB = false;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);
byte gateway[] = {192, 168, 1, 1};
byte subnet[] = {255, 255, 255, 0};
int port = 80;
EthernetUDP udp;
unsigned int localPort = 2390;
char auth[] = "tEKJyMzprhaA_AGYh6X7lF-oMbfW-knz";

int alarmPin = 3;
int alarmPinHigh = 2;
int swiatloWlancznikPinIn = 4;
int swiatloWlancznikPinOut = 5;
int pastuchWlancznikPinIn = 6;
int pastuchWlancznikPinOut = 7;
int lastButtonWlacznik = 0;
int lastButtonLight = 1;
int lastButtonPastuch = 1;
int activeHour = 0;
int deactiveHour = 0;
int x = 0;
int y = 0;
NTPClient timeClient(udp);

WidgetLED ledAlarm(V0);
WidgetLED ledWlacznik(V1);
WidgetLED ledLight(V5);
WidgetLED ledPastuch(V8);

BlynkTimer timer;

void setup() {
  delay(1000);
  Serial.begin(9600);
  pinMode(alarmPin, INPUT);
  pinMode(swiatloWlancznikPinIn, INPUT);
  pinMode(pastuchWlancznikPinIn, INPUT);
  pinMode(alarmPinHigh, OUTPUT);
  pinMode(swiatloWlancznikPinOut, OUTPUT);
  pinMode(pastuchWlancznikPinOut, OUTPUT);
  connectToEthernet();
  timeClient.begin();
  timeClient.setTimeOffset(3600 * 2);
  timeClient.update();
  Blynk.begin(auth);

  timer.setInterval(1000L, sendAlarmPush);
  timer.setInterval(60 * 60 * 1000L, sendWlacznikPush);
  timer.setInterval(300000L, updateTime);
  timer.setInterval(1000L, updateBlynk);
  timer.setInterval(5000L, turnOnLights);
}
void connectToEthernet()
{
  Serial.println("Waiting for ethernet...");
  while (Ethernet.begin(mac) == 0) {}
  Ethernet.setLocalIP(ip);
  Serial.println("Ethernet ready");
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
  delay(1000);
  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());
}
BLYNK_WRITE(V2)
{
  activeHour = param.asInt();
}
BLYNK_WRITE(V3)
{
  deactiveHour = param.asInt();
}
BLYNK_WRITE(V4)
{
  if (lastButtonLight != param.asInt()) {
    lastButtonLight = param.asInt();
    toggleLight();
  }
}

BLYNK_WRITE(V6)
{
  if (lastButtonWlacznik != param.asInt()) {
    lastButtonWlacznik = param.asInt();
    toggleWlacznik();
  }
}
BLYNK_WRITE(V7)
{
  if (lastButtonPastuch != param.asInt()) {
    lastButtonPastuch = param.asInt();
    togglePastuch();
  }
}

void toggleLight() {
  if (digitalRead(swiatloWlancznikPinOut)) {
    digitalWrite(swiatloWlancznikPinOut, 0);
  } else {
    digitalWrite(swiatloWlancznikPinOut, 1);
  }
}
void togglePastuch() {
  
  if (digitalRead(pastuchWlancznikPinOut)) {
    digitalWrite(pastuchWlancznikPinOut, 0);
  } else {
    digitalWrite(pastuchWlancznikPinOut, 1);
  }
}
void toggleWlacznik() {
  if (wlacznik) {
    wlacznik = false;
  } else {
    wlacznik = true;
  }
}
void updateBlynk() {
  Blynk.syncAll();
  if (wlacznik) {
    ledWlacznik.on();
  }
  else {
    ledWlacznik.off();
  }
  if (alarm) {
    ledAlarm.on();
  }
  else {
    ledAlarm.off();
  }

  if (swiatlo) {
    ledLight.on();
  }
  else {
    ledLight.off();
  }
  if (pastuch) {
    ledPastuch.on();
  }
  else {
    ledPastuch.off();
  }
}

void updateTime() {
  timeClient.update();
}

void alarmDetection() {
  digitalWrite(alarmPinHigh, HIGH);
  x = digitalRead(alarmPin);
  if (x != 1) {
    alarm = true;
  }
  else {
    alarm = false;
  }
}

void sendAlarmPush() {
  if (alarm) {
    if (wlacznik) {
      if (checkHours()) {
        Blynk.notify("Alarm");
      }
    }
  }
}

void turnOnLights() {
  if (!swiatlo && alarm) {
    toggleLight();
  }
}
void sendWlacznikPush() {
  if (!wlacznik) {
    Blynk.notify("Powiadomienia są wyłączone");
  }
}
bool checkHours() {

  if (activeHour >= deactiveHour)
  {
    return (timeClient.getHours() >= activeHour || timeClient.getHours() < deactiveHour);
  } else {
    return (timeClient.getHours() >= activeHour && timeClient.getHours() < deactiveHour);
  }
}
void lightControl() {
  if (digitalRead(swiatloWlancznikPinIn)) {
    swiatlo = true;
  } else {
    swiatlo = false;
  }
}
void pastuchControl() {
  if (digitalRead(pastuchWlancznikPinIn)) {
    pastuch = true;
  } else {
    pastuch = false;
  }
}
void loop() {
  updateTime();
  Blynk.run();
  timer.run();
  updateBlynk();
  alarmDetection();
  lightControl();
  pastuchControl();
  // Serial.println((unsigned int) (alarmMillis - millis()));
}
