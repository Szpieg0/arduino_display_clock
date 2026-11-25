#include "RTC.h"
#include <NTPClient.h>
#if defined(ARDUINO_PORTENTA_C33)
#include <WiFiC3.h>
#elif defined(ARDUINO_UNOWIFIR4)
#include <WiFiS3.h>
#endif
#include <WiFiUdp.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include "secrets.h"
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C //For my OLED display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Classes etc.
WiFiUDP Udp;
NTPClient timeClient(Udp);
//Adafruit SHT sensor
#include "Adafruit_SHT4x.h"
Adafruit_SHT4x sht4 = Adafruit_SHT4x();
//Button
const int buttonPin = 2; 

bool needsRefresh = TRUE;

int lastSecond = -1;
uint32_t lastTempReading = 0;
unsigned long lastSync = 0;
bool timeSynced = false;
const unsigned long SYNC_INTERVAL = 600000UL;   // 10 min
const unsigned long RETRY_INTERVAL = 30000UL;    // 30 s
const unsigned long tempInterval = 1000;  //1 second
const int screens = 3; // number of screens


bool connectToWiFi(unsigned long timeoutMs = 20000);

int screenState = 0;
float liveTemp;
float liveHumi;

/*Przygotowanie wszystkich modułów:*/
void setup(){
  Serial.begin(9600);
  Wire.begin();
  pinMode(buttonPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

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
  if (!sht4.begin(&Wire)) {
    display.clearDisplay();
    display.println(F("SHT40 err."));
    display.display();
    delay(2000);
  }
  sht4.setPrecision(SHT4X_MED_PRECISION);
  sensors_event_t humidity, temp;
  sht4.getEvent(&humidity, &temp);
  liveTemp = temp.temperature;
  liveHumi = humidity.relative_humidity;
  clockScreen();

}


void loop() {
  if (digitalRead(buttonPin) == HIGH){
    if(screenState < screens){
      screenState++;
    } else{
      screenState = 0;
    }
    while (digitalRead(buttonPin) == HIGH){
      delay(80);
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }else if (digitalRead(buttonPin) == LOW){
    digitalWrite(LED_BUILTIN, LOW);
  }
  switch (screenState){
    case 0: {
      RTCTime now;
      RTC.getTime(now);

      if (now.getSeconds() != lastSecond) {
      lastSecond = now.getSeconds();
      clockScreen();
      }
      break;
      }
    case 1: {
      if (millis() - lastTempReading >= tempInterval) {
        lastTempReading = millis();
        sensors_event_t humidity, temp;
        sht4.getEvent(&humidity, &temp);
        liveTemp = temp.temperature;
        liveHumi = humidity.relative_humidity;
        }
        tempScreen();
        break;
      }
      case 2: {
        weatherScreen();
        break;
      }
      case 3: {
        sunScreen();
        break;
      }
  
  }
  
  maintainTime();
  maintainWeather();
}



//        FUNCTIONS for all screens:
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

void ntpAlert(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print(F("NTP ERROR!"));
  display.display();
  delay(2000);
}