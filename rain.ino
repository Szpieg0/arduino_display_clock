void rainScreen(){
  if (needsRefresh == true){
  display.clearDisplay();
  display.setTextSize(1);

  if(!weatherValid){
    display.setCursor(0, 28);
    display.print(F("No data!"));
    display.display();
    needsRefresh = false;
    return;
  }

  display.setCursor(0, 0);
  display.print(F("Rain chance: "));
  display.print(rainChance);
  display.print(F(" %"));

  display.setCursor(0, 8);
  display.print(F("Rain precip: "));
  display.print(hourPrecip, 1);
  display.print(F(" mm"));

  display.setCursor(0, 16);
  display.print(F("H.Rain/Shower "));
  display.print(hourRain, 1);
  display.print(F(" "));
  display.print(hourShowers, 1);

  display.setCursor(0, 24);
  display.print(F("Snow "));
  display.print(hourSnowfall, 1);
  display.print(F(" cm Depth "));
  display.print(snowDepth, 0);
  display.print(F("cm"));

  display.setCursor(0, 32);
  display.print(F("Rain: "));
  display.print(rainSum, 1);
  display.print(F(" Shwr: "));
  display.print(showersSum, 1);
  display.setCursor(0, 40);
  display.print(F("Snow: "));
  display.print(snowfallSum, 1);

  display.setCursor(0, 48);
  display.print(F("Pressure: "));
  display.print(pressure, 0);
  display.print(F(" hPa"));

  display.setCursor(0, 56);
  display.print(F("UV max: "));
  display.print(uvMax, 1);
  display.display();
  needsRefresh = false;
  } else {
    return;
  }
}