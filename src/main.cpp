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
const int RXpin = 15,csPin = 13;
int wait, mode;
double test = 0.00;
String oldSpeedStr = "    ";

// const byte speedNums[10][90] = {0xf0, 0xff, 0xff, 0x03, 0xfc, 0xff, 0xff, 0x0f, 0x3f, 0x00, 0x00, 0x3c,
//    0xf3, 0x00, 0x00, 0x30, 0xe3, 0x03, 0x00, 0x30, 0x83, 0x0f, 0x00, 0x30,
//    0x03, 0x1e, 0x00, 0x30, 0x03, 0x78, 0x00, 0x30, 0x03, 0xf0, 0x01, 0x30,
//    0x03, 0xc0, 0x03, 0x30, 0x03, 0x00, 0x0f, 0x30, 0x03, 0x00, 0x3e, 0x30,
//    0x03, 0x00, 0xf8, 0x30, 0x03, 0x00, 0xe0, 0x31, 0x0f, 0x00, 0x80, 0x3f,
//    0xfc, 0xff, 0xff, 0x0f, 0xf0, 0xff, 0xff, 0x03 };

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
    // for (int i=0;i<17;i++) {
    //   byte bitAnd = 1;
    //   for (int j=0;j<30;j++) {
    //     lcd.setPixel(3+i+k*20, 4+j, temp[(i*4) + j/8] & bitAnd);
    //     if (bitAnd == 0x80)
    //       bitAnd = 0x01;
    //     else
    //       bitAnd <<= 1;
    //   }
    // }
  }
  lcd.setCursor(78, 18);
  lcd.draw(point,9,16);
  oldSpeedStr = speedStr;
  // for (int i=0;i<3;i++)
  //   for (int j=0;j<3;j++)
  //     lcd.setPixel(20+i, 31+j, 1);
  //lcd.renderAll();
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
  //lcd.renderAll();
}

void modeClick() {
  lcd.clear();
  lcd.println("Mode Clicked");
  oldSpeedStr = "    ";
  //lcd.renderAll();
}

void setClick() {
  lcd.clear();
  lcd.print("Set Clicked");
  //lcd.renderAll();
  oldSpeedStr = "    ";
  setMode = false;
}
void startSet() {
  if (settable)
    setMode = true;
  lcd.clear();
  lcd.print("Set mode enabled");
  oldSpeedStr = "    ";
//  lcd.renderAll();
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
  wait = millis();
  mode = 0;
  lcd.begin();
	lcd.clear();
  lcd.setColor(255,255,255);
  lcd.setCursor(221,21);
  lcd.draw(knots,84,56);
//  lcd.renderAll();

}

void loop() {
  modeButton.tick();
  setButton.tick();
  char* str = "Hello World";
  File outFile = SD.open("output.txt",FILE_WRITE);
  if (outFile) {
    if (abs(millis()-wait) > 1000) {
      outFile.println(str);
      Serial.println("Sucess");
      //lcd.clear();
      //drawSpeed(0.00);
      //drawUnits();
      //lcd.print("Huzzah");
  //    lcd.renderAll();
      if (test == 10)
        test = 0;
      drawSpeed(test);
      test += .01;
      wait = millis();
    }
    outFile.close();
  } else {
    Serial.println("Could not write file");
    delay(500);
  }
  while (Serial3.available() > 0)
    gps.encode(Serial3.read());
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
  }
}
