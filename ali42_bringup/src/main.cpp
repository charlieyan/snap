#include <Arduino.h>

#include <SPI.h>
#include <SD.h>

#include "sensor.h"

String readString;

void setup() {
  Serial.begin(115200);

  if (InitSensor()) {
    Serial.println("Init : OK");
  } else {
    Serial.println("Init : Not OK");
  }

  // set up supporting pins directions here
  pinMode(VSYNC, INPUT);
  pinMode(WEN, OUTPUT);
  pinMode(RCLK, OUTPUT);
  pinMode(RRST, OUTPUT);

  digitalWriteFast(WEN, HIGH);  // active low, so disables fifo write
  delayMicroseconds(100);

  pinMode(PIXEL0, INPUT);
  pinMode(PIXEL1, INPUT);
  pinMode(PIXEL2, INPUT);
  pinMode(PIXEL3, INPUT);
  pinMode(PIXEL4, INPUT);
  pinMode(PIXEL5, INPUT);
  pinMode(PIXEL6, INPUT);
  pinMode(PIXEL7, INPUT);

  Serial.println("Done");
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read(); // gets one byte from serial buffer
    readString += c; //makes the string readString
    delay(2); //slow looping to allow buffer to fill with next character
  }
  if (readString.length() > 0) {
    Serial.println(readString);
  }

  // sccb tests
  if (readString[0] == 't') {
    for (int j = 0; j < 128; ++j) {
      Serial.println(j);
      StartSCCB();
      byte temp = WriteSCCB(j << 1);
      StopSCCB();
      if (temp > 0) {
        Serial.println("found good one!");
      }
      delay(50);
    }
  } else if (readString[0] == 'r') {
    String rest = readString.substring(1);
    long int x;
    char* pEnd;
    x = strtol(rest.c_str(),&pEnd,16);
    byte temp;
    ReadSensor(x, &temp);
  } else if (readString[0] == 'w') {
    if (readString.length() == 10) {
      String sub_address = readString.substring(1, 5);
      long int sub_address_int;
      char* sub_address_c;
      sub_address_int = strtol(sub_address.c_str(),&sub_address_c,16);

      String data = readString.substring(6, 10);
      long int data_int;
      char* data_c;
      data_int = strtol(data.c_str(),&data_c,16);


      Serial.print("sub_address_int ");
      Serial.println(sub_address_int);

      Serial.print("data_intr ");
      Serial.println(data_int);

      byte temp = WriteSensor(sub_address_int, data_int);
      if (temp > 0) {
        Serial.println("write success!");
      }
    }
  }

  readString = "";
}
