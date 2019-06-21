#ifndef _SENSOR_H_
#define _SENSOR_H_

#include "sccb.h"

int InitSensor(void) {
  int i = 0;
  InitSCCB();
  // maybe add a check InitSensor for manufac id byte high and low
  // did see this which is correct:
  //r0x1C
  //write_sensor_addr: 42
  //read_sensor_addr: 43
  //Got 7F
  //r0x1D
  //write_sensor_addr: 42
  //read_sensor_addr: 43
  //Got A2
  return(0x01);
}

int WriteSensor(byte regID, byte regData) {
  StartSCCB();

  /* phase 1: slave_id is 7 bits + 1 bit 0 = w, 1 = r, + dont care */
  byte write_sensor_addr = sensor_addr << 1;

  // LSB is left 0 for write
  Serial.print("write_sensor_addr: ");
  Serial.println(write_sensor_addr, HEX);

  if (WriteSCCB(write_sensor_addr) == 0) {
    Serial.println("Problem : 3 phase Write Error phase 1.");
    StopSCCB();
    return(0);
  }
  delayMicroseconds(SIO_CLKDELAY);

  /* phase 2 : sub address is just 8 bits, + don't care */
  if (WriteSCCB(regID) == 0) {
    Serial.println("Problem : 3 phase Write Error phase 2.");
    StopSCCB();
    return(0);
  }
  delayMicroseconds(SIO_CLKDELAY);

  /* phase 3 : data writing */
  if (WriteSCCB(regData) == 0) {
    Serial.println("Problem : 3 phase Write Error phase 3.");
    StopSCCB();
    return(0);
  }

  /*exiting*/
  StopSCCB();
  return(1);
}

int ReadSensor(byte regID, byte* regData) {
  StartSCCB();
  /* 2-phase write transmission cycle */
  /*---------------------------------*/

  /* phase 1: slave_id is 7 bits + 1 bit 0 = w, 1 = r, + dont care */
  byte write_sensor_addr = sensor_addr << 1;
  byte read_sensor_addr = write_sensor_addr | 0x01;

  // LSB is left 0 for write
  Serial.print("write_sensor_addr: ");
  Serial.println(write_sensor_addr, HEX);
  Serial.print("read_sensor_addr: ");
  Serial.println(read_sensor_addr, HEX);

  if (WriteSCCB(write_sensor_addr) == 0) {
    Serial.println("ReadSensor failed phase 1-1");
    StopSCCB();
    return(0);
  }
  /* phase 2 : sub-address */
  if (WriteSCCB(regID) == 0) {
    Serial.println("ReadSensor failed phase 1-2");
    StopSCCB();
    return(0);
  }

  /* need to stop and restart between write and read parts */
  StopSCCB();
  StartSCCB();

  /*---------------------------------*/
  /* 2-phase read transmission cycle */
  /* phase 1 : ID address */
  if (WriteSCCB(read_sensor_addr) == 0)
  {
    Serial.println("ReadSensor failed phase 2-1");
    StopSCCB();
    return(0);
  }

  /*phase 2 : data reading*/
  if (ReadSCCB(regData) == 0) {
    Serial.println("ReadSensor failed phase 2-2");
    StopSCCB();
    return(0);
  }

  Serial.print("Got ");
  Serial.println(*regData, HEX);

  /*exiting*/
  StopSCCB();
  return(1);
}

void set_reg(String sub_address, String data) {
  char* sub_address_c;
  long int sub_address_int = strtol(sub_address.c_str(),
    &sub_address_c,16);

  char* data_c;
  long int data_int = strtol(data.c_str(),&data_c,16);

  byte temp = WriteSensor(sub_address_int, data_int);
  if (temp > 0) {
    Serial.println("write success!");
  }
}

void set_reg_2(byte sub_address, byte data) {
  byte temp = WriteSensor(sub_address, data);
  if (temp > 0) {
    Serial.println("write success!");
  }
}

#endif  // _SENSOR_H_
