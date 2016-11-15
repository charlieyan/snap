#include <Wire.h>
#include <stdio.h>
#include <stdlib.h>     /* strtol */
#include "Sccb.h"

#define SIOC_PIN 19
#define SIOD_PIN 18

#define XCLK  3 // 8 MHz output
#define VSYNC 4
#define HREF  5
#define PCLK  6

#define D6    7
#define D7    8
#define D4    9
#define D5    10
#define D2    11
#define D3    12
#define D0    13
#define D1    14
#define Y0    15
#define Y1    16  
#define PWDN  17
#define RST   20

#define LED   21
#define SWI   22

#define OV2640_SCCB_ADDRESS 0x30 /* 0011 110 0 = 60, but Wire takes care of last bit so 30 */

String readString;

Sccb* sccb_ptr;

void setup() {
  Serial.begin(115200);  // start serial for output
  sccb_ptr = new Sccb(SIOC_PIN, SIOD_PIN, OV2640_SCCB_ADDRESS);
  delay(1000);
  analogWriteFrequency(XCLK, 8000000); // Create 8 MHz clock output for XCLK, which drives the PLL that goes downstreams to timing etc.
  analogWrite(XCLK, 127);; // 50% duty cycle
  delay(100);
}
void loop() {

}


