void weatherScreen(){
  if (needsRefresh == true){
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print(F("Warsaw"));

  if(!weatherValid){
    display.setCursor(0, 28);
    display.print(F("No data!"));
    display.display();
    needsRefresh = false;
    return;
  }

  display.setTextSize(3);
  display.setCursor(0, 14);
  display.print(weatherTemp, 1);
  display.drawCircle(display.getCursorX() + 4, 16, 3, SSD1306_WHITE);
  display.setCursor(display.getCursorX() + 12, 14);
  display.print(F("C"));

  display.setTextSize(1);
  display.setCursor(0, 44);
  display.print(weatherCodeText(weatherCode));

  display.setCursor(0, 56);
  display.print(F("Wind : "));
  display.print(weatherWind, 0);
  display.print(F(" km/h"));
  display.display();
  needsRefresh = false;
  } else {
    return;
  }
}