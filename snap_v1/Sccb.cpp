#include <Wire.h>
#include <Arduino.h>
#include "Sccb.h"

Sccb::Sccb(int sccb_sioc_pin, int sccb_siod_pin, int device_id) {
  Wire.begin();
  int found_device = scanForDevices();
  if (found_device == device_id) {
    _device_id = device_id;
  }
}

// This function will scan the i2c bus for a valid device
// only expecting to find one in this application and that
// is the Camera's SCCB interface
int Sccb::scanForDevices(void) {
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

bool Sccb::device_found(void) {
  return _device_id > 0;
}

// Read a single byte from address and return it as a byte
byte Sccb::readRegister(int device_id, byte address) {
  byte data;
  Serial.print("reading from ");
  Serial.println(address);
  Wire.beginTransmission(device_id);
  Wire.write(address);
  Wire.endTransmission();
  Wire.requestFrom(device_id, 1);
  data = Wire.read();
  int result = Wire.endTransmission();
  Serial.print("endTransmission ends with ");
  Serial.println(result);
  return data;
}

// Writes a single byte (data) into address
int Sccb::writeRegister(int device_id, byte address, byte data) {
  Wire.beginTransmission(device_id);
  Wire.write(address);
  Wire.write(data);
  return Wire.endTransmission();
}

// Write an array of register-value entries to the SCCB interface
int Sccb::writeRegisterArray(int device_id, const struct regval_list *vals) {
  while (vals->reg != 0xff || vals->val != 0xff) {
    int ret = writeRegister(device_id, vals->reg, vals->val);
    if (ret < 0) {
      return ret;
    }
    vals++;
  }
  return 0;
}

