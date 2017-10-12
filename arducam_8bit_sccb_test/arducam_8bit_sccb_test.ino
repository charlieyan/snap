#include <SPI.h>
#include <ArduCAM.h>

#define SIO_C 23
#define SIO_D 22
#define M_CLK 21

#define SPI_CS 10 // slave select for SPI

ArduCAM myCAM(SIO_C, SIO_D, SPI_CS);

String readString;

void setup()
{
  Serial.begin(115200);
  Serial.println("starting");

  pinMode(M_CLK, OUTPUT);
  analogWriteFrequency(M_CLK, 12000000); // 15 MHz
  analogWrite(M_CLK, 128);

  myCAM.InitSCCB();
  int sensor_address = myCAM.ScanSCCB();
  if (sensor_address > -1) {
    Serial.print("assigned sensor_addr_ ");
    Serial.println(sensor_address);
    myCAM.sensor_addr_ = sensor_address;
  }
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
    x = strtol(rest.c_str(),&pEnd, 16);
    byte temp;
    myCAM.rdSensorReg8_8(x, &temp);
    Serial.print("got ");
    Serial.println(temp, HEX);
  }
  else if (readString[0] == 'w') {
    if (readString.length() == 10) { // 12 for 2 bytes
      String sub_address = readString.substring(1, 5); // 1, 5 for 1 byte, 7 for 2 bytes
      long int sub_address_int;
      char* sub_address_c;
      sub_address_int = strtol(sub_address.c_str(),&sub_address_c,16);

      String data = readString.substring(6, 10); // 7, 10 for 1 byte, 8,12 for 2
      long int data_int;
      char* data_c;
      data_int = strtol(data.c_str(),&data_c,16);

      Serial.print("sub_address_int ");
      Serial.println(sub_address_int);
      Serial.print("data_int ");
      Serial.println(data_int);

      myCAM.wrSensorReg8_8(sub_address_int, data_int);
    }
  }

  readString = "";
}
