void sunScreen(){
  if (needsRefresh == TRUE){
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(F("Sun, Warsaw"));

  display.setTextSize(2);
  display.setCursor(0, 20);
  display.print(F("sunrise "));
  display.print(sunriseStr);

  display.setCursor(0, 44);
  display.print(F("sunshine "));
  display.print(sunsetStr);
  display.display();
  bool needsRefresh = FALSE;
  } else {
  return;
  }
}