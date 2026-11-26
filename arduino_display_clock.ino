//Constant libraries
#if defined(ARDUINO_PORTENTA_C33)
#include <WiFiC3.h>
#elif defined(ARDUINO_UNOWIFIR4)
#include <WiFiS3.h>
#endif
#include "RTC.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
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

WiFiUDP Udp;
NTPClient timeClient(Udp, "pl.pool.ntp.org"); //NTP server

#include "Adafruit_SHT4x.h"
Adafruit_SHT4x sht4 = Adafruit_SHT4x();
//Button
const int buttonPin = 2; 
bool lastButtonState = false;
bool needsRefresh = true;

//variables for time sync
int lastSecond = -1;
unsigned long lastTempReading = 0;
unsigned long lastSync = 0;
time_t syncEpoch = 0;
unsigned long syncMillis = 0;
bool timeSynced = false;
const unsigned long SYNC_INTERVAL = 1800000UL;   // 10 min
const unsigned long RETRY_INTERVAL = 30000UL;    // 30 s
const unsigned long tempInterval = 1000;  //1 second

const int screens = 4; // number of screens + 1
int screenState = 0;

bool connectToWiFi(unsigned long timeoutMs = 20000);
//temp variables
float liveTemp;
float liveHumi;

//Przygotowanie wszystkich modułów:

void setup(){
  Serial.begin(9600);
  Wire1.begin();
  pinMode(buttonPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
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

  timeClient.begin();
  timeSynced = syncTime();
  lastSync = millis();
  if (!sht4.begin(&Wire1)) {
    display.clearDisplay();
    display.println(F("SHT40 err."));
    display.display();
    delay(4000);
  }
  sht4.setPrecision(SHT4X_MED_PRECISION);
  sensors_event_t humidity, temp;
  sht4.getEvent(&humidity, &temp);
  liveTemp = temp.temperature;
  liveHumi = humidity.relative_humidity;
  clockScreen();
}


void loop() {
  if (digitalRead(buttonPin) == LOW && lastButtonState == true){
    lastButtonState = false;
  }
  if (digitalRead(buttonPin) == HIGH && lastButtonState == false){
    if(screenState < screens){
      screenState++;
    } else{
      screenState = 0;
    }
    needsRefresh = true;
    lastButtonState = true;
  }
  
  switch (screenState){
    case 0: {
      int sec = utcNow() % 60;
      if (sec != lastSecond) {
        lastSecond = sec;
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
        tempScreen();
        }
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
    case 4: {
      rainScreen();
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
