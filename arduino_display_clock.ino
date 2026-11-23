int lastSecond = -1;
uint32_t ostatniOdczytTemp = 0;
unsigned long lastSync = 0;
bool timeSynced = false;
const unsigned long SYNC_INTERVAL = 600000UL;   // 10 min
const unsigned long RETRY_INTERVAL = 30000UL;    // 30 s

bool connectToWiFi(unsigned long timeoutMs = 20000);

int screenState = 0;

/*Przygotowanie wszystkich modułów:*/
void setup(){
  Serial.begin(9600);
  Wire.begin();
  pinMode(buttonPin, INPUT);

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
    displaydisplay();
    delay(2000);
  }
  sht4.setPrecision(SHT4X_MED_PRECISION);
  sensors_event_t humidity, temp;
  sht4.getEvent(&humidity, &temp);
  celcius = temp.temperature;
  clockScreen();

}


void loop() {
  if (buttonPin == 1){
    if(screenState < 3){
      screenState++;
    } else{
      screenState = 0;
    }
    while (buttonPin == 1){
      delay(80);
    }
  }
  switch (screenState){
    case 0:
      RTCTime now;
      RTC.getTime(now);

      if (now.getSeconds() != lastSecond) {
      lastSecond = now.getSeconds();
      clockScreen();
      break;
    case 1:
      if (millis() - ostatniOdczytTemp >= interwalMs) {
        ostatniOdczytTemp = millis();
        sensors_event_t humidity, temp;
        sht4.getEvent(&humidity, &temp);
        aktualnaTemperatura = temp.temperature;
  }
      tempScreen();
      break;
  
  }
  
  }
  maintainTime();
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