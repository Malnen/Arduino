#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <NTPClient.h>
#include <BlynkSimpleEthernet.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
#include <SPI.h>
#include <SD.h>
#include <LinkedList.h>
bool alarm = false;
bool wlacznik = true;
bool swiatlo = false;
bool pastuch = false;
bool sdInitialized = false;
bool toggleLightB = false;
bool alarmStringWritten = false;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);
byte gateway[] = {192, 168, 1, 1};
byte subnet[] = {255, 255, 255, 0};
int port = 80;
EthernetUDP udp;
unsigned int localPort = 2390;
char auth[] = "tEKJyMzprhaA_AGYh6X7lF-oMbfW-knz";

LinkedList<String> alarmHistory;

File alarmListFile;
char fileName[] = "alarm.txt";
unsigned long long lastConnectionMillis = 0;
unsigned long long reconnectTime = 120000;
const int alarmPin = 3;
const int alarmPinStatus = 2;
const int swiatloWlancznikPinIn = 15;//before it was 4 but cspin is now 4
const int swiatloWlancznikPinOut = 5;
const int pastuchWlancznikPinIn = 6;
const int pastuchWlancznikPinOut = 9; //7 8 zlamany
const int ethernetResetPin = 14;
int lastButtonWlacznik = 0;
int lastButtonLight = 1;
int lastButtonPastuch = 1;
int activeHour = 0;
int deactiveHour = 0;
int x = 0;
int y = 0;
const int SDPin = 4;
const int W5100Pin = 10;
const int historySize = 20;

WidgetLED ledAlarm(V0);
WidgetLED ledWlacznik(V1);
WidgetLED ledLight(V5);
WidgetLED ledPastuch(V8);
WidgetLED ledSD(V12);

BlynkTimer timer;
WidgetRTC rtc;
WidgetTable table;
BLYNK_ATTACH_WIDGET(table, V10);

void setup() {
  delay(1000);
  Serial.begin(9600);

  pinMode(alarmPin, INPUT);
  pinMode(swiatloWlancznikPinIn, INPUT);
  pinMode(pastuchWlancznikPinIn, INPUT);
  pinMode(alarmPinStatus, OUTPUT);
  pinMode(swiatloWlancznikPinOut, OUTPUT);
  pinMode(pastuchWlancznikPinOut, OUTPUT);
  pinMode(ethernetResetPin, OUTPUT);
  pinMode(SDPin, OUTPUT);
  pinMode(W5100Pin, OUTPUT);
  pinMode(53, OUTPUT);


  connectToEthernet();

  Blynk.begin(auth);

  timer.setInterval(1000L, sendAlarmPush);
  timer.setInterval(60 * 60 * 1000L, sendWlacznikPush);
  timer.setInterval(1000L, updateBlynk);
  timer.setInterval(1000L, updateTimeDisplay);
  timer.setInterval(60000L, updateTable);
  // timer.setInterval(5000L, turnOnLights);
}

BLYNK_CONNECTED() {
  // Synchronize time on connection
  rtc.begin();
}

void updateTable() {
  //table.clear();
  for (int i = 0; i < alarmHistory.size(); i++) {
    String temp = alarmHistory.get(alarmHistory.size() - 1 - i);
    int index = 0;
    index = temp.indexOf(",");
    table.addRow(i, temp.substring(0, index), temp.substring(index + 1));
  }
}

void setupSD() {
  digitalWrite(W5100Pin, HIGH);
  digitalWrite(SDPin, LOW);
  Serial.print(F("Initializing SD card..."));

  if (!SD.begin(SDPin)) {
    Serial.println(F("initialization failed!"));
    sdInitialized = false;
    return;
  }
  Serial.println(F("initialization done."));

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  alarmListFile = SD.open(fileName);

  // if the file opened okay, write to it:
  if (alarmListFile) {
    sdInitialized = true;
    String temp = "";
    while (alarmListFile.available()) {
      char c = alarmListFile.read();
      if (c != '\n') {
        temp += c;
      } else {
        //Serial.println(temp);
        alarmHistory.add(temp);
        temp = "";
        if (alarmHistory.size() > historySize) {
          alarmHistory.shift();
        }
      }
    }

    alarmListFile.close();
  } else {
    // if the file didn't open, print an error:
    sdInitialized = false;
    Serial.print(F("error opening "));
    Serial.print(fileName);
  }
  digitalWrite(SDPin, HIGH);
  digitalWrite(W5100Pin, LOW);
}

void connectToEthernet()
{
  digitalWrite(ethernetResetPin, LOW);
  delay(500);
  digitalWrite(ethernetResetPin, HIGH);

  setupSD(); // sd card is on ethrenet shield so reinitializing

  Serial.println(F("Waiting for ethernet..."));
  while (Ethernet.begin(mac) == 0) {}
  Ethernet.setLocalIP(ip);
  Serial.println(F("Ethernet ready"));
  Serial.print(F("My IP address: "));
  Serial.println(Ethernet.localIP());
  delay(1000);
  Serial.println(F("Starting UDP"));
  udp.begin(localPort);
  Serial.print(F("Local port: "));
  Serial.println(String(udp.localPort()));
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

void setLed(WidgetLED led, bool b) {
  if (b) {
    led.on();
  }
  else {
    led.off();
  }
}

void updateBlynk() {
  Blynk.syncAll();
  setLed(ledWlacznik, wlacznik);
  setLed(ledLight, swiatlo);
  setLed(ledAlarm, alarm);
  setLed(ledPastuch, pastuch);
  setLed(ledSD, sdInitialized );
  Blynk.virtualWrite(V11, FreeRam());

}


void alarmDetection() {
  if (wlacznik && checkHours()) {
    digitalWrite(alarmPinStatus, HIGH);
  } else {
    digitalWrite(alarmPinStatus, LOW);
  }
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
        Blynk.notify(F("Alarm"));
        if (!alarmStringWritten) {
          alarmStringWritten = true;
          writeToSD(F("Alarm"));
        }
      }
    }
  } else {
    if (alarmStringWritten) {
      alarmStringWritten = false;
      writeToSD(F("Wyłączono"));
    }
  }
}

void writeToSD(String s) {
  digitalWrite(W5100Pin, HIGH);
  digitalWrite(SDPin, LOW);
  alarmListFile = SD.open(fileName, FILE_WRITE);

  // if the file opened okay, write to it:
  if (alarmListFile) {
    sdInitialized = true;
    alarmListFile.println(getTimeString() + " " + getDateString() + "," + s);
    alarmHistory.add(getTimeString() + " " + getDateString() + "," + s);
    if (alarmHistory.size() > historySize) {
      alarmHistory.shift();
    }
    alarmListFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.print(F("error opening "));
    Serial.print(fileName);
    sdInitialized = false;
  }
  digitalWrite(SDPin, HIGH);
  digitalWrite(W5100Pin, LOW);
}

void turnOnLights() {
  if (!swiatlo && alarm && wlacznik) {
    toggleLight();
  }
}
void sendWlacznikPush() {
  if (!wlacznik) {
    Blynk.notify(F("Powiadomienia są wyłączone"));
  }
}
bool checkHours() {

  if (activeHour >= deactiveHour)
  {
    return (hour() >= activeHour || hour() < deactiveHour);
  } else {
    return (hour() >= activeHour && hour() < deactiveHour);
  }
}
String getTimeString() {
  String h = String(hour());
  String m = String(minute());
  String s = String(second());

  if (h.length() < 2) {
    h = "0" + h;
  }
  if (m.length() < 2) {
    m = "0" + m;
  }
  if (s.length() < 2) {
    s = "0" + s;
  }
  return h + ":" + m + ":" + s;
}

String getDateString() {
  String y = String(year());
  String m = String(month());
  String d = String(day());

  if (m.length() < 2) {
    m = "0" + m;
  }
  if (d.length() < 2) {
    d = "0" + d;
  }
  return y + "." + m + "." + d;
}
void lightControl() {
  if (digitalRead(swiatloWlancznikPinIn)) {
    swiatlo = true;
  } else {
    swiatlo = false;
  }
}
void updateTimeDisplay() {
  Blynk.virtualWrite(V9, getTimeString());
}
void pastuchControl() {
  if (digitalRead(pastuchWlancznikPinIn)) {
    pastuch = true;
  } else {
    pastuch = false;
  }
}
void loop() {
  Ethernet.maintain();

  //  updateTime();
  Blynk.run();
  timer.run();
  updateBlynk();
  alarmDetection();
  lightControl();
  pastuchControl();
  
  if (!Blynk.connected()) {
    Blynk.connect();
    if (millis() - lastConnectionMillis > reconnectTime) {
      connectToEthernet();
      lastConnectionMillis = millis();
    }
  }
  else {
    lastConnectionMillis = millis();
  }

}
