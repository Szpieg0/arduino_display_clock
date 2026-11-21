#include "RTC.h"
#include <NTPClient.h>
#if defined(ARDUINO_PORTENTA_C33)
#include <WiFiC3.h>
#elif defined(ARDUINO_UNOWIFIR4)
#include <WiFiS3.h>
#endif
#include <WiFiUdp.h> //WiFi Librares

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> //Display Libraries
#include "secrets.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C //For this display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


WiFiUDP Udp;
NTPClient timeClient(Udp);

int lastSecond = -1;

unsigned long lastSync = 0;
bool timeSynced = false;
const unsigned long SYNC_INTERVAL = 1200000UL;   // 1 h
const unsigned long RETRY_INTERVAL = 30000UL;    // 30 s

bool connectToWiFi(unsigned long timeoutMs = 20000);

void setup(){
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  delay(300);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.println(F("Connecting"));
  display.display();

  if (!connectToWiFi()) {
  display.clearDisplay();
  display.setCursor(0, 20);
  display.println(F("No WiFi"));
  display.display();
  delay(4000);
  }

  //RTC & NTP section
  RTC.begin();
  timeClient.begin();
  timeSynced = syncTime();
  lastSync = millis();
  clockScreen();

}
void loop() {

  RTCTime now;
  RTC.getTime(now);

  if (now.getSeconds() != lastSecond) {
    lastSecond = now.getSeconds();
    clockScreen();
  }
  maintainTime();
}



//        FUNCTIONS:
bool connectToWiFi(unsigned long timeoutMs) {
  unsigned long start = millis();
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - start > timeoutMs) {
      Serial.println("WiFi timeout");
      return false;
    }
    delay(500);
  }
  Serial.println("Connected");
  return true;
}

String twoDigits(int value) {
  if (value < 10) {
    return "0" + String(value);
  }
  return String(value);
}

String formatTime(int hour, int minute) {
  return twoDigits(hour) + ":" + twoDigits(minute);
}

bool syncTime() {
  for (int i = 0; i < 3; i++) {
    if (timeClient.forceUpdate()) {
      RTCTime utcTime(timeClient.getEpochTime());   // UTC
      RTC.setTime(utcTime);
      Serial.println("NTP sync OK");
      return true;
    }
    delay(500);
  }
  Serial.println("NTP sync FAILED");
  ntpAlert();
  return false;
}

//      LIVE Functions:
void clockScreen(){
  RTCTime currentTime = getLocalTime();
  int currentHour = currentTime.getHour();
  int currentMinutes = currentTime.getMinutes();
  int currentSeconds = currentTime.getSeconds();
  display.clearDisplay();
  display.setTextSize(4);
  display.setCursor(0, 15);
  display.print(formatTime(currentHour, currentMinutes));
  display.setTextSize(1);
  display.setCursor(115, 50);
  display.print(twoDigits(currentSeconds));
  display.display();
}

bool isSummerTime(int y, int m, int d, int h) {   // All in UTC
  if (m < 3 || m > 10) return false;
  if (m > 3 && m < 10) return true;
  if (m == 3) {
    int lastSun = 31 - (5 * y / 4 + 4) % 7;
    return d > lastSun || (d == lastSun && h >= 1);
  }
  int lastSun = 31 - (5 * y / 4 + 1) % 7;          // October
  return d < lastSun || (d == lastSun && h < 1);
}

RTCTime getLocalTime() {
  RTCTime utc;
  RTC.getTime(utc);
  bool summer = isSummerTime(utc.getYear(), Month2int(utc.getMonth()), utc.getDayOfMonth(), utc.getHour());
  time_t local = utc.getUnixTime() + (summer ? 7200 : 3600);
  return RTCTime(local);
}

void ntpAlert(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print(F("NTP ERROR!"));
  display.display();
  delay(2000);
}

void maintainTime() {
  unsigned long interval = timeSynced ? SYNC_INTERVAL : RETRY_INTERVAL;
  if (millis() - lastSync < interval) return;
  lastSync = millis();

  if (WiFi.status() != WL_CONNECTED) {
    if (!connectToWiFi(10000)) return;
  }
  if (syncTime()) timeSynced = true;
}


