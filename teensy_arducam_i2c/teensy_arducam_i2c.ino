// ArduCAM demo (C)2014 Lee
// web: http://www.ArduCAM.com
// This program is a demo of how to use most of the functions
// of the library with a supported camera modules, and can run on any Arduino platform.
//
// This demo was made for Omnivision OV2640 2MP sensor.
// It will run the ArduCAM as a real 2MP digital camera, provide both preview and JPEG capture.
// The demo sketch will do the following tasks:
// 1. Set the sensor to BMP preview output mode.
// 2. Switch to JPEG mode when shutter buttom pressed.
// 3. Capture and buffer the image to FIFO.
// 4. Store the image to Micro SD/TF card with JPEG format.
// 5. Resolution can be changed by myCAM.set_JPEG_size() function.
// This program requires the ArduCAM V3.1.0 (or later) library and Rev.C ArduCAM shield
// and use Arduino IDE 1.5.2 compiler or above

#include <Wire.h>
#include <SPI.h>
#define OV2640_SCCB_ADDRESS 0x30 /* 0011 110 0 = 60, but Wire takes care of last bit so 30 */

const int SPI_CS = 10;
int found_address;
String readString;
byte regDat;

// This function will scan the i2c bus for a valid device
// only expecting to find one in this application and that
// is the Camera's SCCB interface
int scanForDevices(void) {
  for (int address = 1; address <= 127; address++) {
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

// Read a single byte from address and return it as a byte
byte readRegister(byte device_id, byte address) {
  byte data;
  Serial.print("reading from ");
  Serial.println(address);
  Wire.beginTransmission(device_id);
  Wire.write(address);
//  Wire.endTransmission();
  Wire.requestFrom(device_id, (byte)1);
  data = Wire.read();
  int result = Wire.endTransmission();
  Serial.print("endTransmission ends with ");
  Serial.println(result);
  return data;
}

int writeRegister(int device_id, byte address, byte data) {
  Wire.beginTransmission(device_id);
  Wire.write(address);
  Wire.write(data);
  return Wire.endTransmission();
}

void setup()
{
  Serial.begin(115200);
  Serial.println("starting ArduCAM");
  Wire.begin();

  found_address = scanForDevices();
  Serial.println(found_address);
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

  if (readString == "status") {
    Serial.println(found_address);
  }
  else if (readString[readString.length()-1] == 'c') {
    readString = "";
  }
  else if (readString[0] == 'r') {
    String rest = readString.substring(1);
    long int x;
    char* pEnd;
    x = strtol(rest.c_str(),&pEnd,16);
    Serial.println(x);
    byte bank_select = readRegister((byte)found_address, x);
    Serial.println(bank_select);
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

      int result = writeRegister((byte)found_address, (byte)sub_address_int, (byte)data_int);
      Serial.print("writing ");
      Serial.print(data_int);
      Serial.print(" to ");
      Serial.print(sub_address_int);
      Serial.print(" finished with ");
      Serial.println(result);
    }
  }
}




