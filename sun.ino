void sunScreen(){
  if (needsRefresh == true){
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(F("Warsaw"));

  display.setTextSize(1);
  display.setCursor(0, 20);
  display.print(F("sunrise "));
  display.print(sunriseStr);

  display.setCursor(0, 44);
  display.print(F("sunset "));
  display.print(sunsetStr);
  display.display();
  needsRefresh = false;
  } else {
  return;
  }
}