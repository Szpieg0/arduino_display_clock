void tempScreen(){
  display.clearDisplay();
  display.setTextSize(3);
  display.setCursor(0,0);
  display.print(liveTemp);
  display.setCursor(0,25);
  display.print(liveHumi, 1);
  display.print(" %");
  display.display();
  delay(100);
}