#include <SPI.h>
#include <Wire.h>

#include "Camera.h"
#include "Sccb.h"

#define HWSERIAL Serial1 // TX = 1, RX = 0 // set this to the hardware serial port you wish to use

int ledPin =  13; //Status LED connected to digital pin 13
const byte XCLK  =  3; // 8 MHz output
const byte VSYNC =  4;
const byte HREF  =  5;
const byte PCLK  =  6;
const byte D7    = 20;
const byte D6    = 17;
const byte D5    = 16;
const byte D4    = 15;
const byte D3    = 14;
const byte D2    =  9;
const byte D1    =  8;
const byte D0    =  7;
const byte RESET = 21;
Camera camera = Camera(VSYNC, HREF, PCLK, D0, D1, D2, D3, D4, D5, D6, D7, RESET);

#define SCCB_ADDRESS 0x21
Sccb sccb = Sccb(SCCB_ADDRESS);

void setup() {
  Serial.begin(115200);  // start serial for output
  HWSERIAL.begin(115200);
  pinMode(ledPin, OUTPUT);
  delay(1000); //Wait a second for OpenLog to init

  // Create 8 MHz clock output 50% duty cycle for Camera's XCLK
  analogWriteFrequency(XCLK, 8000000);
  analogWrite(XCLK, 127);
  delay(100);

  // Release hardware reset line on OV7670
  camera.setCameraReset(true);
  delay(100);

  // Configure the camera for operation
//  sccb.initOV7670();
//  sccb.slowFrameRate();

  // OpenLog issue
  HWSERIAL.println(); 
  HWSERIAL.println("Run OpenLog Test"); 
  int testAmt = 1;
  for(int numofTests = 0 ; numofTests < testAmt ; numofTests++)
  {
    for(int k = 33; k < 43 ; k++) //This loop will print 100 lines of 110 characters each
    {
      HWSERIAL.write(k); //Print the ASCII value directly: ! then " then #, etc to *
      HWSERIAL.println("charlie");
      for(int i = 1 ; i < 10 ; i++)
      {
        HWSERIAL.print(i, DEC);
        HWSERIAL.println("charlie");
      }
      if(digitalRead(ledPin) == 0) //Turn the status LED on/off as we go
        digitalWrite(ledPin, HIGH);
      else
        digitalWrite(ledPin, LOW);
    }
  } //End numofTests loop
  unsigned long totalCharacters = (long)testAmt * 100 * 110;
  HWSERIAL.print("Characters pushed: ");
  HWSERIAL.println(totalCharacters);  
  HWSERIAL.print("Time taken (s): ");
  HWSERIAL.println(millis()/1000);
  HWSERIAL.println("Done!");
}

void loop() {
//  camera.get_rgb_image();
  
  int incomingByte;
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    Serial.print("USB received: ");
    Serial.println(incomingByte, DEC);
    HWSERIAL.print("USB received:");
    HWSERIAL.println(incomingByte, DEC);
  }
  if (HWSERIAL.available() > 0) {
    incomingByte = HWSERIAL.read();
    Serial.print("UART received: ");
    Serial.println(incomingByte, DEC);
    HWSERIAL.print("UART received:");
    HWSERIAL.println(incomingByte, DEC);
  }

  //Blink the Status LED because we're done!
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay(100);
}
