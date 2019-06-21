#ifndef __PIN_MAPPING_H_
#define __PIN_MAPPING_H_

#define sensor_addr 33 // 0x21 in HEX, recheck with 't'
#define SIO_C 23
#define SIO_D 22

#define SIO_CLKDELAY 100

// supporting pins
//#define VSYNC 14
#define VSYNC 19
#define WEN 15
#define OE 16
#define RRST 17
#define WRST 18
#define RCLK 9 // CONFIRMED WITH SCHEMATIC FROM ROBODYN

// pixel data pins
#define PIXEL0 2
#define PIXEL1 14
#define PIXEL2 7
#define PIXEL3 8
#define PIXEL4 6
#define PIXEL5 20
#define PIXEL6 21
#define PIXEL7 5

void printBinary(byte inByte) {
  for (int b = 7; b >= 0; b--) {
    Serial.print(bitRead(inByte, b));
  }
}

void enableWrite() {
  digitalWriteFast(WEN, LOW);
}

void disableWrite() {
  digitalWriteFast(WEN, HIGH);
}

void resetReadPointer() {
  digitalWriteFast(RRST, LOW);
  delayMicroseconds(100);
  digitalWriteFast(RRST, HIGH);
}

#endif  // __PIN_MAPPING_H_
