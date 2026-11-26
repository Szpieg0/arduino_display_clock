//Main Clock Screen
void clockScreen(){
  if (syncEpoch == 0) {
  display.clearDisplay();
  display.setTextSize(4);
  display.setCursor(0, 15);
  display.print(F("--:--"));
  display.display();
  return;
}
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


String formatTime(int hour, int minute) {
  return twoDigits(hour) + ":" + twoDigits(minute);
}


bool syncTime() {
  if (timeClient.forceUpdate()) {
  syncEpoch = timeClient.getEpochTime();
  syncMillis = millis();
  return true;
}
  return false;
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
  RTCTime utc(utcNow());
  bool summer = isSummerTime(utc.getYear(), Month2int(utc.getMonth()), utc.getDayOfMonth(), utc.getHour());
  time_t local = utc.getUnixTime() + (summer ? 7200 : 3600);
  return RTCTime(local);
}

time_t utcNow() {
  return syncEpoch + (millis() - syncMillis) / 1000;
}

void maintainTime() {
  unsigned long interval = timeSynced ? SYNC_INTERVAL : RETRY_INTERVAL;
  if (millis() - lastSync < interval) return;
  lastSync = millis();

  display.fillCircle(0, 60, 3, SSD1306_WHITE);
  display.display();

  if (WiFi.status() != WL_CONNECTED) {
    if (!connectToWiFi(10000)) return;
  }
  if (syncTime()) {
    timeSynced = true;
  }
}