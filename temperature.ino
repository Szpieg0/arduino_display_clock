void tempScreen(){
  display.clearDisplay();
  display.setTextSize(3);
  display.setCursor(0,0);
  display.print(liveTemp, 1);
  display.drawCircle(display.getCursorX() + 4, 3, 3, SSD1306_WHITE);
  display.setCursor(display.getCursorX() + 12, 0);
  display.print(F("C"));

  display.setCursor(0,25);
  display.print(liveHumi, 1);
  display.print(" %");
  display.display();
  delay(100);
}