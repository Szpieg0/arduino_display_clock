const char WEATHER_HOST[] = "api.open-meteo.com";
const int  WEATHER_PORT   = 443;
const char WEATHER_PATH[] =
  "/v1/forecast?latitude=52.2297&longitude=21.0122"
  "&current=temperature_2m,wind_speed_10m,weather_code"
  "&hourly=precipitation_probability,precipitation,rain,showers,snowfall,snow_depth,pressure_msl"
  "&daily=sunrise,sunset,uv_index_max,rain_sum,showers_sum,snowfall_sum"
  "&forecast_days=1&forecast_hours=1&timezone=Europe%2FWarsaw";

const unsigned long WEATHER_INTERVAL = 900000UL;   // 15 minutes
const unsigned long WEATHER_RETRY    = 60000UL;    // 1 min after error

float weatherTemp = 0;    //Temp in celcius now
float weatherWind = 0;    // Wind Now in km/h
int   weatherCode = 0;    //WMO Code
int   rainChance   = 0;   // %
float hourPrecip   = 0;   // mm (rain + showers + snow)
float hourRain     = 0;   // mm
float hourShowers  = 0;   // mm
float hourSnowfall = 0;   // cm
float snowDepth    = 0;   // cm API give in meters
float pressure     = 0;   // hPa
float uvMax        = 0;
float rainSum      = 0;   // mm, daily
float showersSum   = 0;   // mm, daily
float snowfallSum  = 0;   // cm, daily
String sunriseStr = "--:--";     // sunrise
String sunsetStr  = "--:--";     // sunset
bool weatherValid = false;
bool weatherOk = false;   //last check of API
unsigned long lastWeatherTry = 0;

bool fetchWeather() {
  display.fillCircle(0, 61, 3, SSD1306_WHITE);
  display.display();

  WiFiSSLClient client;
  client.setTimeout(5000);
  if (!client.connect(WEATHER_HOST, WEATHER_PORT)) {
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
  JsonObject hr  = doc["hourly"];
  JsonObject day = doc["daily"];
  const char* sr = doc["daily"]["sunrise"][0];     // "2026-11-23T07:19"
  const char* ss = doc["daily"]["sunset"][0];
  if (cur.isNull() || hr.isNull() || !sr || !ss) return false;

  weatherTemp = cur["temperature_2m"];
  weatherWind = cur["wind_speed_10m"];             // km/h
  weatherCode = cur["weather_code"];
  rainChance   = hr["precipitation_probability"][0];
  hourPrecip   = hr["precipitation"][0];
  hourRain     = hr["rain"][0];
  hourShowers  = hr["showers"][0];
  hourSnowfall = hr["snowfall"][0];
  snowDepth    = hr["snow_depth"][0].as<float>() * 100;   // m -> cm
  pressure     = hr["pressure_msl"][0];
  uvMax        = day["uv_index_max"][0];
  rainSum      = day["rain_sum"][0];
  showersSum   = day["showers_sum"][0];
  snowfallSum  = day["snowfall_sum"][0];
  sunriseStr  = String(sr).substring(11, 16);
  sunsetStr   = String(ss).substring(11, 16);
  weatherValid = true;
  return true;
}

void maintainWeather() {

  unsigned long interval = weatherOk ? WEATHER_INTERVAL : WEATHER_RETRY;
  if (lastWeatherTry != 0 && millis() - lastWeatherTry < interval) return;
  lastWeatherTry = millis();
  if (WiFi.status() != WL_CONNECTED) { weatherOk = false; return; }
  weatherOk = fetchWeather();
  needsRefresh = true;
}

const char* weatherCodeText(int code) {   // WMO codes
    switch (code) {
    case 0:  return "Clear sky";
    case 1:  return "Mainly clear";
    case 2:  return "Partly cloudy";
    case 3:  return "Overcast";
    case 45: case 48: return "Fog";
    case 51: case 53: case 55: return "Drizzle";
    case 56: case 57: return "Freezing drizzle";
    case 61: case 63: case 65: return "Rain";
    case 66: case 67: return "Freezing rain";
    case 71: case 73: case 75: return "Snow fall";
    case 77: return "Snow grains";
    case 80: case 81: case 82: return "Rain showers";
    case 85: case 86: return "Snow showers";
    case 95: return "Thunderstorm";
    case 96: case 99: return "Thunderstorm with hail";
    default: return "Unknown code";
  }
}