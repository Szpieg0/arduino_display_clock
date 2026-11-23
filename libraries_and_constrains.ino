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

//Weather
const char WEATHER_HOST[] = "api.open-meteo.com";
const int WEATHER_PORT = 443;
const char WEATHER_PATH[] = "https://api.open-meteo.com/v1/forecast?latitude=52.2297&longitude=21.0122&current=temperature_2m,wind_speed_10m,precipitation,weather_code&timezone=Europe/Warsaw";
