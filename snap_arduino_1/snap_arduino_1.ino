#include <SPI.h> // communicate with SPI devices
#include <Wire.h> // communicate with I2C devices

#include <ArduCAM.h>
#include <memorysaver.h>
#include <ov2640_regs.h>

const int CS = 10; // set pin 10 as the slave select
ArduCAM ov2640(OV2640, CS); // sensor_addr = 0x60, makes CS pin output, P_CS is port, B_CS is bitmask

void setup() {
  Wire.begin();
  
  Serial.begin(115200);
  Serial.println("ArduCAM Start!");

  Serial.println("initialize SPI");
  SPI.begin();

  Serial.println("testing ArduCAM SPI bus");
  ov2640.write_reg(ARDUCHIP_TEST1, 0x55);
  uint8_t temp = ov2640.read_reg(ARDUCHIP_TEST1);
  if(temp != 0x55)
  {
    Serial.println("SPI interface Error!");
    while(1);
  }
  
  Serial.println("making sure module is OV2640");
  uint8_t vid,pid;
  ov2640.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
  ov2640.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
  if((vid != 0x26) || (pid != 0x42))
    Serial.println("Can't find OV2640 module!");
  else
    Serial.println("OV2640 detected");

  ov2640.set_format(BMP); // .bmp, alternatively use JPEG
  ov2640.InitCAM();
}

void loop() {

}

