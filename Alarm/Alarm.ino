#include <SPI.h>
#include <Ethernet.h>
#include <Time.h>
#include <TimeLib.h>
#include <EthernetUdp.h>
#include <NTPClient.h>

bool alarm = false;
bool wlacznik = true;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 150);
byte gateway[] = {192, 168, 1, 1};
byte subnet[] = {255, 255, 255, 0};
int port = 80;
EthernetUDP udp;
unsigned int localPort = 2390;
unsigned long long alarmMillis;
unsigned long long timeMillis;

int alarmPin = 7;
int alarmPinHigh = 2;
int wlacznikPin = 9;
int wlacznikPinHigh = 4;

int x = 0;
int y = 0;
NTPClient timeClient(udp);
char DEVID1[] = "v2BA5CD1D541DE2E";
char serverName[] = "api.pushingbox.com";

int days;
int months;
int years;

int hours;
int minutes;
int seconds;
EthernetServer server(80);
EthernetClient client;

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
  setTime(timeClient.getEpochTime());
  server.begin();
  setBootTime();
  alarmMillis = millis();
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

void setBootTime() {
  days = day();
  months = month();
  years = year();
  hours = hour();
  minutes = minute();
  seconds = second();
}
void website() {
  EthernetClient webClient = server.available();
  if (webClient) {
    boolean currentLineIsBlank = true;
    while (webClient.connected()) {
      if (webClient.available()) {
        char c = webClient.read();
        if (c == '\n' && currentLineIsBlank) {
          webClient.println("HTTP/1.1 200 OK");
          webClient.println("Content-Type: text/html");
          webClient.println("Connection: close");
          webClient.println();
          webClient.println("<!DOCTYPE HTML>");
          webClient.println("<html>");
          webClient.println("<meta http-equiv=\"refresh\" content=\"5\">");
          webClient.print("<h2>Arduino UNO<br>");
          webClient.print("<small> Czas uruchomienia: ");
          if (days < 10) {
            webClient.print("0");
            webClient.print(days);
          }
          else {
            webClient.print(days);
          }
          webClient.print(".");
          if (months < 10) {
            webClient.print("0");
            webClient.print(months);
          }
          else {
            webClient.print(months);
          }
          webClient.print(".");
          webClient.print(years);

          webClient.print(" ");
          if (hours < 10) {
            webClient.print("0");
            webClient.print(hours);
          }
          else {
            webClient.print(hours);
          }
          webClient.print(":");
          if (minutes < 10) {
            webClient.print("0");
            webClient.print(minutes);
          }
          else {
            webClient.print(minutes);
          }
          webClient.print(":");
          if (seconds < 10) {
            webClient.print("0");
            webClient.print(seconds);
          }
          else {
            webClient.print(seconds);
          }
          webClient.print("</small><br>");
          webClient.print("<small> Aktualny czas: " + timeClient.getFormattedTime() + "</small>");
          webClient.print("</h2><br>");
          webClient.print("<h3>Pin 7 = ");
          webClient.print(x);
          webClient.println("<br>");
          webClient.print("<h3>Pin 9 = ");
          webClient.print(y);
          webClient.println("<br>");
          webClient.println("Status: ");
          if (alarm) {
            webClient.println("<font color=\"red\">Przerwa w ogrodzeniu!</font>");
          } else {
            webClient.println("<font color=\"green\">Polaczono</font>");
          }
          webClient.println("<br>Wlacznik: ");
          if (!wlacznik) {
            webClient.println("<font color=\"red\">OFF</font>");
          } else {
            webClient.println("<font color=\"green\">ON</font>");
          }
          webClient.println("</h2>");
          webClient.print("<br><br><br><br>");
          webClient.println("</html>");
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    delay(1);
    webClient.stop();
  }
}


void loop() {
  if (timeMillis + 300000 < millis()) {
    timeClient.update();
    timeMillis = millis();
  }
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

  if (y != 1) {
    wlacznik = false;
  }
  else {
    wlacznik = true;
  }
  if (alarmMillis  < millis()) {
    if (alarm && wlacznik) {
      if (timeClient.getHours() >= 21  || timeClient.getHours() <= 8) {
        sendToPushingBoxPower(DEVID1);
      }
      //alarmMillis = millis() + 60000;
    }
  }
  website();
  //Serial.println((unsigned int) (alarmMillis - millis()));
}
