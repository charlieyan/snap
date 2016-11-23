#include <Wire.h>
#include <stdio.h>
#include <stdlib.h>     /* strtol */
#include "Sccb.h"
#include "ov2640_regs.h"

#define OV2640_SCCB_ADDRESS 0x30 /* 0011 110 0 = 60, but Wire takes care of last bit so 30 */

#define SCL_PIN 18
#define SDA_PIN 19

#define VSYNC 20
#define HREF  21
#define PCLK  17

#define D7    16
#define D6    23
#define D5    15
#define D4    4
#define D3    14
#define D2    5
#define D1    13
#define D0    6

#define Y1    12
#define Y0    7

#define PWDN  -1 // not connected
#define RST   11
#define XCLK  22 // 8 MHz output

#define TX    1 // RX1 on openlog 
#define RX    0 // TX0 on openlog

#define SWI   2
#define LED   3

String readString;
Sccb sccb = Sccb();
bool pulseLED = false;
bool ledOn = false;

// OV7670 register values
const struct regval_list ov2640_regs[] = {
  {BANK_SEL,    BANK_SEL_SENS},
  {COM7,        COM7_SRST}, // system reset
  {0xFF, 0xFF}  // End marker
};

void setup_pins() {
  pinMode(VSYNC,  INPUT);
  attachInterrupt(digitalPinToInterrupt(VSYNC), vsync_rising_cb, RISING);
  attachInterrupt(digitalPinToInterrupt(VSYNC), vsync_falling_cb, FALLING);
  pinMode(HREF,   INPUT);
  pinMode(PCLK,   INPUT);
  pinMode(D7,     INPUT);
  pinMode(D6,     INPUT);
  pinMode(D5,     INPUT);
  pinMode(D4,     INPUT);
  pinMode(D3,     INPUT);
  pinMode(D2,     INPUT);
  pinMode(D1,     INPUT);
  pinMode(D0,     INPUT);
  pinMode(Y1,     INPUT);
  pinMode(Y0,     INPUT);

  pinMode(RST,    OUTPUT);
  pinMode(XCLK,   OUTPUT);

  pinMode(LED,    OUTPUT);
  pinMode(SWI,    INPUT);

  pinMode(TX,     OUTPUT);
  pinMode(RX,     INPUT);  
}

void vsync_rising_cb() {
  Serial.println("vsync rising");
}

void vsync_falling_cb() {
  Serial.println("vsync falling");
}

void setup() {
  Wire.begin();
  setup_pins();
  
  delay(100);
  analogWriteFrequency(XCLK, 8000000); // Create 8 MHz clock output for XCLK, which drives the PLL that goes downstreams to timing etc.
  analogWrite(XCLK, 127);; // 50% duty cycle
  delay(100);

  int found_address = sccb.scanForDevices();
  if (found_address == OV2640_SCCB_ADDRESS) {
    sccb._device_id = found_address;
  }

  Serial.begin(115200);  // start serial for debug output
  delay(1000);
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

  if (readString == "status") {
    Serial.println(sccb._device_id);
  }
  else if (readString == "led") {
    pulseLED = !pulseLED;
  }
  else if (readString[0] == 'r') {
    String rest = readString.substring(1);

    long int x;
    char* pEnd;
    x = strtol(rest.c_str(),&pEnd,16);
    Serial.println(x);

    byte bank_select = sccb.readRegister(OV2640_SCCB_ADDRESS, x);
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

      int result = sccb.writeRegister(OV2640_SCCB_ADDRESS, (byte)sub_address_int, (byte)data_int);
      Serial.print("writing to ");
      Serial.print(sub_address_int);
      Serial.print(" finished with ");
      Serial.println(result);
    }
  }

  readString = "";

  if (pulseLED) {
    if (!ledOn) {
      digitalWrite(LED, HIGH);
    }
    else {
      digitalWrite(LED, LOW); 
    }
    ledOn = !ledOn;
  }
}


