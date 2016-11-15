#include <SPI.h>
#include <Wire.h>
#include <stdio.h>
#include <stdlib.h>     /* strtol */

#define SIO_C 11
#define SIO_D 12

#define T_CYC 10 // datasheet says T_CYC = 10 microseconds
#define T_CYC_2 5
#define T_CYC_3 2
#define T_PSA 1
#define T_PRA 5 // T_PSA min 0, T_PRA is 1.25

#define XCLK  3 // 8 MHz output
#define VSYNC 4
#define HREF  5
#define PCLK  6

#define BANK_SEL              0xFF /* Register Bank Select */
#define BANK_SEL_DSP          0x00 /* Table 12 in the docs */
#define BANK_SEL_SENS         0x01 /* Table 13 in the docs */

#define SIO_D_HIGH HIGH
#define SIO_D_LOW LOW
String readString;
String sccbReadString;

void setup() {
  Serial.begin(115200);  // start serial for output
  delay(1000);
  analogWriteFrequency(XCLK, 4000000); // Create 8 MHz clock output 50% duty cycle for Camera's XCLK
  analogWrite(XCLK, 127);
  delay(100);
  
  pinMode(SIO_D, INPUT); // master keeps the SIO_D tri-stated when off => high impedeance => INPUT, and pin is pulled up to 3.3 V with 4.7k?
  pinMode(SIO_C, OUTPUT); // single-direction, active-high, master must keep this high when off
}

bool test = false;

void loop() {
  digitalWrite(SIO_C, HIGH);
  while (Serial.available()) {
    char c = Serial.read(); // gets one byte from serial buffer
    readString += c; //makes the string readString
    delay(2); //slow looping to allow buffer to fill with next character
  }
  if (readString.length() > 0) {
    Serial.println(readString);
  }
  if (readString == "write") {
    bitBangSccbWrite();
  }
  else if (readString == "read") {
    bitBangSccbRead();
  }
  else if (readString[0] == 't') {
    test = !test;
  }

  if (test) {
    bitBangSccbRead();
    delayMicroseconds(T_CYC * 6);
  }
  readString = ""; //empty for next input
}

void start_sccb() {
  pinMode(SIO_D, OUTPUT); // SIO_D = output, master drives SIO_D to logical 1
  digitalWrite(SIO_D, SIO_D_HIGH);
  delayMicroseconds(T_PSA);
  delayMicroseconds(T_PRA);
  digitalWrite(SIO_C, LOW);
  digitalWrite(SIO_D, SIO_D_LOW); // using SIO_D as OUTPUT, this starts communication, maybe don't need tri-state?
}

void end_sccb() {
  // end sccb data transmission
  digitalWrite(SIO_C, HIGH);
  pinMode(SIO_D, OUTPUT);
  digitalWrite(SIO_D, SIO_D_HIGH);
}

void write_out_byte(byte data, int start_offset, int end_offset) {
  for (int j = start_offset; j < end_offset; j++ ) { // Lowest 7 bits are written
    if( (data << j) & 0x80 ) {
      digitalWrite(SIO_D, SIO_D_HIGH);
    }
    else {
      digitalWrite(SIO_D, SIO_D_LOW);
    }
    delayMicroseconds(T_CYC_3); // gap between setting data and rising clock to count it
    digitalWrite(SIO_C, HIGH);
    delayMicroseconds(T_CYC);
    digitalWrite(SIO_C, LOW);
  }
}

void handle_dont_care_bit() {
//  digitalWrite(SIO_D, INPUT_PULLUP); // optional
//  delayMicroseconds(T_CYC_3); // for the don't care bit
//  byte result = 0x00;
//  if (digitalRead(SIO_D) == HIGH) {
//    result = 0x01;
//    Serial.println("success");
//  }
//  pinMode(SIO_D, OUTPUT);
  delayMicroseconds(T_CYC); // for the don't care bit
}

byte bitBangSccbWrite() {
  Serial.println("bitBangSccbWrite begin");
  start_sccb();
  bool good_slave_id = sccb_phase_1(0x60, true);
  bool good_subaddress = sccb_phase_2(BANK_SEL, true);
  bool good_write_data = sccb_phase_3(0x01);
  end_sccb();
  Serial.println("bitBangSccbWrite end");
  byte result = 0x00;
  return result;
}

bool sccb_phase_1(byte m_data, bool write_or_read) { // array todo?
  bool success = false;
  write_out_byte(m_data, 1, 8); // low 7 bits for the slave id
  // R/W bit
  if (write_or_read) {
    digitalWrite(SIO_D, SIO_D_HIGH);
  }
  else {
    digitalWrite(SIO_D, SIO_D_LOW);
  }
  delayMicroseconds(T_CYC_2);
  digitalWrite(SIO_C, HIGH);
  delayMicroseconds(T_CYC_2);
  digitalWrite(SIO_C, LOW);
  handle_dont_care_bit(); // Dont-care bit
  digitalWrite(SIO_D, SIO_D_LOW);
  delayMicroseconds(T_CYC_2);
  return success;
}

byte sccb_phase_2(byte m_data, bool write_or_read) {
  byte result = 0x00;
  if (write_or_read) {
    write_out_byte(m_data, 0, 8); // 8 bits for the sub-address
    // Dont-care bit
    handle_dont_care_bit();
    digitalWrite(SIO_D, SIO_D_LOW);
    delayMicroseconds(T_CYC_2);
  }
  else {
    Serial.println("reading!");
    pinMode(SIO_D, INPUT);
    pinMode(SIO_C, INPUT);
    sccbReadString = "";    
  }
  return result;
}

bool sccb_phase_3(byte m_data) { // only master, for writing to slave, same as phase 2 write
    write_out_byte(m_data, 0, 8); // 8 bits for the data
    // Dont-care bit
    handle_dont_care_bit();
    digitalWrite(SIO_D, SIO_D_LOW);
    delayMicroseconds(T_CYC_2);
    return true;
}

void bitBangSccbRead() {
  Serial.println("bitBangSccbRead begin");
  start_sccb();
  bool good_slave_id = sccb_phase_1(0x60, true);
  bool good_subaddress = sccb_phase_2(BANK_SEL, true);
  end_sccb();
  start_sccb();
  sccb_phase_1(0x60, false);
  good_subaddress = sccb_phase_2(BANK_SEL, false);
  end_sccb();
  Serial.println("bitBangSccbRead end");
  byte result = 0x00;
}

