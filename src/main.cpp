#include "Arduino.h"
#include <stdlib.h>
#include <SPI.h>
#include <SD.h>
#include "line.h"
#include "OneButton.h"
//#include "PCD8544_SPI.h"
#include "MultiLCD.h"
#include "gps_text.h"

typedef byte uint8_t;

TinyGPSPlus gps;
//PCD8544_SPI_FB lcd;
LCD_ILI9341 lcd;
OneButton modeButton(26,0);
OneButton setButton(27,0);
bool settable = false, setMode = false;
const int RXpin = 15,csPin = 13,interval = 200;
int oldTime, mode;
double test = 0.00;
String oldSpeedStr = "    ";

const PROGMEM byte point [18] = {
  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF,
};

void drawSpeed(double speed) {
  String speedStr = String(speed);
  for (int k=0;k<3;k++) {
    if (speedStr[k+!!k] == oldSpeedStr[k+!!k])
      continue;
    const byte* temp = speedNums[speedStr[k+!!k] - '0'];
    lcd.setXY(6-((k/2)*3)+80*k, 0);
    lcd.draw(temp,70,168);
  }
  lcd.setCursor(78, 18);
  lcd.draw(point,9,16);
  oldSpeedStr = speedStr;
}

void drawUnits() {
  if (mode == 0) {
    for (int i=0;i<20;i++) {
      byte bitAnd = 1;
      for (int j=0;j<14;j++) {
        lcd.setPixel(62+i,29+j,knots[i*2+j/8] & bitAnd);
        if (bitAnd == 0x80)
          bitAnd = 1;
        else;
          bitAnd <<= 1;
        }
    }
  }
}

void modeClick() {
  lcd.clear();
  lcd.println("Mode Clicked");
  oldSpeedStr = "    ";
}

void setClick() {
  lcd.clear();
  lcd.print("Set Clicked");
  oldSpeedStr = "    ";
  setMode = false;
}
void startSet() {
  if (settable)
    setMode = true;
  lcd.clear();
  lcd.print("Set mode enabled");
  oldSpeedStr = "    ";
}

void setup() {
  pinMode(csPin, OUTPUT);
  modeButton.attachClick(modeClick);
  setButton.attachClick(setClick);
  setButton.attachDuringLongPress(startSet);
  if (!SD.begin(csPin))
    Serial.println("Card Failure");
  Serial3.begin(4800);
  Serial.begin(115200);
  oldTime = millis();
  mode = 0;
  lcd.begin();
	lcd.clear();
  lcd.setColor(255,255,255);
  lcd.setCursor(221,21);
  lcd.draw(knots,84,56);
}

void loop() {
  modeButton.tick();
  setButton.tick();
  // char* str = "Hello World";
  // File outFile = SD.open("output.txt",FILE_WRITE);
  // int newTime = millis();
  // if (outFile) {
  //   if (abs(newTime-oldTime) > interval) {
  //     outFile.println(str);
  //     //Serial.println(gps.location.rawLat().deg);
  //     if (test == 10.0)
  //       test = 0;
  //     drawSpeed(test);
  //     test += .01;
  //     oldTime = newTime;
  //   }
  //   outFile.close();
  // } else {
  //   Serial.println("Could not write file");
  //   delay(500);
  // }
  while (Serial3.available() > 0) {
    char tmp = Serial3.read();
    gps.encode(tmp);
  }
  if (gps.location.isUpdated()) {
    Serial.print(F("LOCATION   Fix Age="));
    Serial.print(gps.location.age());
    Serial.print(F("ms Raw Lat="));
    Serial.print(gps.location.rawLat().negative ? "-" : "+");
    Serial.print(gps.location.rawLat().deg);
    Serial.print("[+");
    Serial.print(gps.location.rawLat().billionths);
    Serial.print(F(" billionths],  Raw Long="));
    Serial.print(gps.location.rawLng().negative ? "-" : "+");
    Serial.print(gps.location.rawLng().deg);
    Serial.print("[+");
    Serial.print(gps.location.rawLng().billionths);
    Serial.print(F(" billionths],  Lat="));
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(" Long="));
    Serial.println(gps.location.lng(), 6);
    Serial.print("Speed: ");
    Serial.println(gps.speed.knots(),3);
    drawSpeed(gps.speed.knots());
  }
}
