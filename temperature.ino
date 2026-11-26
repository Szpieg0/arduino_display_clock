void tempScreen(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("live temp and humi");
  display.setTextSize(3);
  display.setCursor(0,10);
  display.print(liveTemp, 1);
  display.drawCircle(display.getCursorX() + 4, 13, 3, SSD1306_WHITE);
  display.setCursor(display.getCursorX() + 12, 10);
  display.print(F("C"));

  display.setCursor(0,35);
  display.print(liveHumi, 1);
  display.print(" %");
  display.display();
}