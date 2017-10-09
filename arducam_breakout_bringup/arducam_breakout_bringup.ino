#include <i2c_t3.h>
#include <SPI.h>
#include <ArduCAM.h>
#include "memorysaver.h"

#define I2C_ADDR 0x30
#define SPI_CS 10 // slave select for SPI

ArduCAM myCAM(OV2640, SPI_CS);

String readString;

void setup()
{
  Serial.begin(115200);
  Serial.println("starting");

  Wire.begin();
//
//  delay(1000);
//  pinMode(SPI_CS, OUTPUT);
//  SPI.begin();
//  myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
//  uint8_t temp = myCAM.read_reg(ARDUCHIP_TEST1);
//  if (temp != 0x55) {
//    Serial.println("SPI interface Error!");
//    while (1);
//  } else {
//    Serial.println("myCam SPI ready");
//  }
}

// This function will scan the i2c bus for a valid device
// only expecting to find one in this application and that
// is the Camera's SCCB interface
int scanForDevices(void) {
  for (int address = 1; address <= 256; address++) {
    // This scanner uses the return value of
    // the Wire.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    int error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("i2C device found at: 0x");
      Serial.println(address, HEX);
      return address;
    }
  }
  Serial.println("No i2C devices found");
  return 0;
}
 
void loop()
{
  while (Serial.available()) {
    char c = Serial.read(); // gets one byte from serial buffer
    readString += c; //makes the string readString
    delay(2); //slow looping to allow buffer to fill with next character
  }
  if (readString.length() > 0) {
    Serial.println(readString);
  }

  if (readString[0] == 'r') {
    String rest = readString.substring(1);
    long int x;
    char* pEnd;
    x = strtol(rest.c_str(),&pEnd,16);

    uint8_t vid;
    myCAM.rdSensorReg8_8((uint8_t)x, &vid);
    Serial.println(vid);
  }
  else if (readString[0] == 's') {
    uint8_t vid;
    for (int i = 0; i < 256; i++) {
      myCAM.rdSensorReg8_8((uint8_t)i, &vid);
      if (vid > 0) {
        Serial.print(i, HEX);
        Serial.print(" ");
        Serial.println(vid, HEX);
      }
    }
  }
  else if (readString[0] == 'x') {
    int found_address = scanForDevices();
    Serial.println(found_address);
  }
  else if (readString[0] == 'w') {
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

      myCAM.wrSensorReg8_8(sub_address_int, data_int);

      uint8_t vid;
      myCAM.rdSensorReg8_8((uint8_t)sub_address_int, &vid);
      Serial.println(vid);
    }
  }
  else if (readString[0] == 'y') {
      myCAM.wrSensorReg8_8(0xFF, 1);
  }
  readString = "";

  myCAM.wrSensorReg8_8(0xFF, 1);
  
//  myCAM.wrSensorReg8_8(0xFF, 0x7F);
//  uint8_t vid;
//  myCAM.rdSensorReg8_8(0xFF, &vid);
}
