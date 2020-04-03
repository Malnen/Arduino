#include <SPI.h>
#include <Ethernet.h>
#include <Time.h>
#include <EthernetUdp.h>
#include <NTPClient.h>
#include <BlynkSimpleEthernet.h>

bool alarm = false;
bool wlacznik = true;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);
byte gateway[] = {192, 168, 1, 1};
byte subnet[] = {255, 255, 255, 0};
int port = 80;
EthernetUDP udp;
unsigned int localPort = 2390;
unsigned long long alarmMillis;
unsigned long long timeMillis;
unsigned long long blynkMillis;

char auth[] = "tEKJyMzprhaA_AGYh6X7lF-oMbfW-knz";

int alarmPin = 7;
int alarmPinHigh = 2;
int wlacznikPin = 9;
int wlacznikPinHigh = 4;

int activeHour = 0;
int deactiveHour = 0;
int x = 0;
int y = 0;
NTPClient timeClient(udp);
char DEVID1[] = "v2BA5CD1D541DE2E";
char serverName[] = "api.pushingbox.com";

EthernetClient client;

WidgetLED ledAlarm(V0);
WidgetLED ledWlacznik(V1);

void setup() {
  delay(1000);
  Serial.begin(9600);
  pinMode(alarmPin, INPUT);
  pinMode(wlacznikPin, INPUT);
  pinMode(wlacznikPinHigh, OUTPUT);
  pinMode(alarmPinHigh, OUTPUT);
  connectToEthernet();
  timeClient.begin();
  timeClient.setTimeOffset(3600 * 2);
  timeClient.update();
  alarmMillis = millis();
  blynkMillis = millis();
  Blynk.begin(auth);
  digitalWrite(8, HIGH);
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
void sendToPushingBoxPower(char devid[]) {
  client.stop();

  if (client.connect(serverName, 80)) {
    client.print("GET /pushingbox?devid=");
    client.print(devid);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(serverName);
    client.println("User-Agent: Arduino");
    client.println();

    alarmMillis = millis() + 60000;
  }
}


BLYNK_WRITE(V2)
{
  activeHour = param.asInt();
}
BLYNK_WRITE(V3)
{
  deactiveHour = param.asInt();
}

void updateBlynk() {
  if (blynkMillis < millis()) {
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
    blynkMillis = millis() + 1000;
  }
}

void updateTime() {
  if (timeMillis + 300000 < millis()) {
    timeClient.update();
    timeMillis = millis();
  }
}
void alarmDetection() {
  digitalWrite(alarmPinHigh, HIGH);
  digitalWrite(wlacznikPinHigh, HIGH);
  x = digitalRead(alarmPin);
  y = digitalRead(wlacznikPin);
  if (x != 1) {
    alarm = true;
  }
  else {
    alarm = false;
  }

  if (digitalRead(8) != 1) {
    wlacznik = false;
  }
  else {
    wlacznik = true;
  }
  if (alarmMillis  < millis()) {
    if (alarm && wlacznik) {
      if (activeHour >= deactiveHour) {
        if (timeClient.getHours() >= activeHour || timeClient.getHours() <= deactiveHour) {
          sendToPushingBoxPower(DEVID1);
        }
      }
      else {
        if (timeClient.getHours() >= activeHour && timeClient.getHours() <= deactiveHour) {
          sendToPushingBoxPower(DEVID1);
        }
        //alarmMillis = millis() + 1000;
      }
    }
  }
}
void loop() {
  updateTime();
  alarmDetection();
  Blynk.run();
  updateBlynk();

 // Serial.println((unsigned int) (alarmMillis - millis()));
}
