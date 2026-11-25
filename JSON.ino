const char WEATHER_HOST[] = "api.open-meteo.com";
const int  WEATHER_PORT   = 443;
const char WEATHER_PATH[] =
  "/v1/forecast?latitude=52.2297&longitude=21.0122"
  "&current=temperature_2m,wind_speed_10m,weather_code"
  "&daily=sunrise,sunset&forecast_days=1&timezone=Europe%2FWarsaw";

const unsigned long WEATHER_INTERVAL = 900000UL;   // 15 minutes
const unsigned long WEATHER_RETRY    = 60000UL;    // 1 min after error

float weatherTemp = 0;
float weatherWind = 0;
int   weatherCode = 0;
String sunriseStr = "--:--";     // sunrise
String sunsetStr  = "--:--";     // sunshine
bool weatherValid = false;
bool weatherOk = false;   //last check of API
unsigned long lastWeatherTry = 0;

bool fetchWeather() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(fetchWeather);
  display.display();
  WiFiSSLClient client;
  client.setTimeout(5000);
  if (!client.connect(WEATHER_HOST, WEATHER_PORT)) {
    Serial.println(F("Error of Weather API module"));
    return false;
  }

  client.print(F("GET "));
  client.print(WEATHER_PATH);
  client.print(F(" HTTP/1.0\r\nHost: "));
  client.print(WEATHER_HOST);
  client.print(F("\r\nConnection: close\r\n\r\n"));

  if (!client.find("\r\n\r\n")) { client.stop(); return false; }   // skip headers

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, client);
  client.stop();
  if (err) {
    Serial.print(F("Weather: JSON error: "));
    Serial.println(err.c_str());
    return false;
  }

  JsonObject cur = doc["current"];
  const char* sr = doc["daily"]["sunrise"][0];     // "2026-11-23T07:19"
  const char* ss = doc["daily"]["sunset"][0];
  if (cur.isNull() || !sr || !ss) return false;

  weatherTemp = cur["temperature_2m"];
  weatherWind = cur["wind_speed_10m"];             // km/h
  weatherCode = cur["weather_code"];
  sunriseStr  = String(sr).substring(11, 16);      // "07:19"
  sunsetStr   = String(ss).substring(11, 16);
  weatherValid = true;

  Serial.print(F("Weather OK: "));
  Serial.print(weatherTemp); Serial.print(F(" C, "));
  Serial.print(weatherWind); Serial.print(F(" km/h, code "));
  Serial.print(weatherCode); Serial.print(F(", "));
  Serial.print(sunriseStr);  Serial.print(F(" / "));
  Serial.println(sunsetStr);
  return true;
}

void maintainWeather() {

  unsigned long interval = weatherOk ? WEATHER_INTERVAL : WEATHER_RETRY;
  if (lastWeatherTry != 0 && millis() - lastWeatherTry < interval) return;
  lastWeatherTry = millis();
  if (WiFi.status() != WL_CONNECTED) { weatherOk = false; return; }
  weatherOk = fetchWeather();
  bool needsRefresh = TRUE;
}

const char* weatherCodeText(int code) {   // WMO codes in Polish
  switch (code) {
    case 0:  return "Bezchmurnie";
    case 1:  return "Prawie bezchmurnie";
    case 2:  return "Czesciowe zachm.";
    case 3:  return "Pochmurno";
    case 45: case 48: return "Mgla";
    case 51: case 53: case 55: return "Mzawka";
    case 56: case 57: return "Marznaca mzawka";
    case 61: case 63: case 65: return "Deszcz";
    case 66: case 67: return "Marznacy deszcz";
    case 71: case 73: case 75: return "Snieg";
    case 77: return "Ziarna sniegu";
    case 80: case 81: case 82: return "Przelotny deszcz";
    case 85: case 86: return "Przelotny snieg";
    case 95: return "Burza";
    case 96: case 99: return "Burza z gradem";
    default: return "Nieznany kod";
  }
}