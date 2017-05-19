// -------------------------------------------------------------------------------------------
// Basic Master
// -------------------------------------------------------------------------------------------
//
// This creates a simple I2C Master device which when triggered will send/receive a text 
// string to/from a Slave device.  It is intended to pair with a Slave device running the 
// basic_slave sketch.
//
// Pull pin12 input low to send.
// Pull pin11 input low to receive.
//
// This example code is in the public domain.
//
// -------------------------------------------------------------------------------------------

#include <i2c_t3.h>

// Memory
#define MEM_LEN 1
char databuf[MEM_LEN];
String readString;
int found_address;

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
      Serial.print("new i2C device found at: 0x");
      Serial.println(address, HEX);
      return address;
    }
  }
  Serial.println("No i2C devices found");
  return 0;
}

byte readRegister(byte device_id, byte address) {
  byte data;
  Serial.print("reading from ");
  Serial.println(address);

  Wire.beginTransmission(found_address);
  Wire.write(address);
  Wire.sendTransmission(I2C_NOSTOP);

  Wire.requestFrom(device_id, (size_t)1);
  data = Wire.read();
  return data;
}

int writeRegister(int device_id, byte address, byte data) {
  Wire.beginTransmission(device_id);
  Wire.write(address);
  Wire.write(data);
  return Wire.endTransmission();
}

void print_i2c_status(void)
{
    switch(Wire.status())
    {
    case I2C_WAITING:  Serial.print("I2C waiting, no errors\n"); break;
    case I2C_ADDR_NAK: Serial.print("Slave addr not acknowledged\n"); break;
    case I2C_DATA_NAK: Serial.print("Slave data not acknowledged\n"); break;
    case I2C_ARB_LOST: Serial.print("Bus Error: Arbitration Lost\n"); break;
    case I2C_TIMEOUT:  Serial.print("I2C timeout\n"); break;
    case I2C_BUF_OVF:  Serial.print("I2C buffer overflow\n"); break;
    default:           Serial.print("I2C busy\n"); break;
    }
}

void setup()
{
  Serial.println("starting");
  Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, 100000);
  memset(databuf, 0, sizeof(databuf));
  Serial.begin(115200);

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
    found_address = scanForDevices();
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
    print_i2c_status();
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
      print_i2c_status();
    }
  }
  readString = "";
}

