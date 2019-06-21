#ifndef _SCCB_H_
#define _SCCB_H_

#include "reg_addresses.h"
#include "pin_mapping.h"

void InitSCCB(void) {
  pinMode(SIO_C, OUTPUT);
  pinMode(SIO_D, OUTPUT);

  digitalWrite(SIO_C, HIGH);
  digitalWrite(SIO_D, HIGH);
  Serial.println(
    "InitSCCB - Port Direction and set high.");
}

void StartSCCB(void) {
  // Serial.println("Starting SCCB...");
  /*put HIGH in order to avoid propagating unknown bus state.*/
  digitalWrite(SIO_D, HIGH);
  delayMicroseconds(SIO_CLKDELAY);
  digitalWrite(SIO_C, HIGH);
  delayMicroseconds(SIO_CLKDELAY);
  digitalWrite(SIO_D, LOW);
  // documentation: assert SIO_D low while SIO_C high
  delayMicroseconds(SIO_CLKDELAY);
  digitalWrite(SIO_C, LOW);
  // Serial.println("Starting SCCB : done.");
}

void StopSCCB(void) {
  // Serial.println("Stoping SCCB...");
  digitalWrite(SIO_D,LOW);
  delayMicroseconds(SIO_CLKDELAY);
  digitalWrite(SIO_C,HIGH);
  delayMicroseconds(SIO_CLKDELAY);
  digitalWrite(SIO_D,HIGH);
  delayMicroseconds(SIO_CLKDELAY);
  // Serial.println("Stoping SCCB : done.");
}

char WriteSCCB(byte m_data) {
  unsigned char tem;
  for(int j = 0;j < 8;j++) {
    /*write the MSB not written yet :*/
    if((m_data << j) & 0x80)
    {
      digitalWrite(SIO_D, HIGH);
    }
    else {
      digitalWrite(SIO_D, LOW);
    }
    delayMicroseconds(SIO_CLKDELAY);
    digitalWrite(SIO_C, HIGH);
    delayMicroseconds(SIO_CLKDELAY);
    digitalWrite(SIO_C, LOW);
    delayMicroseconds(SIO_CLKDELAY);
  }
  /*eight bits have been send, let's deal with the ninth Don't-care one*/
  /*we put the SIO_D pin at low in order to avoid propagating any unknown state.*/
  /*why input ? because it enables a high impedance, I suppose...?*/
  pinMode(SIO_D, INPUT);
  digitalWrite(SIO_D, LOW);

  delayMicroseconds(SIO_CLKDELAY);
  digitalWrite(SIO_C, HIGH);
  delayMicroseconds(SIO_CLKDELAY);

  /*there is something to read to check the transmission :*/
  if(digitalRead(SIO_D) == HIGH) {
    // Serial.println("SCCB Write NG");
    tem = 0;
  } else {
    tem = 1;
    // Serial.println("SCCB Write : OK.");
  }

  digitalWrite(SIO_C, LOW);
  delayMicroseconds(SIO_CLKDELAY);

  /*let's clean that up :*/
  pinMode(SIO_D, OUTPUT);

  return tem;
}

char ReadSCCB(byte* m_data) {
  /* Let's make things readable */
  pinMode(SIO_D, INPUT);
  digitalWrite(SIO_D, LOW);

  for (unsigned int j = 0; j < 8; j++) {
    delayMicroseconds(SIO_CLKDELAY);
    digitalWrite(SIO_C, HIGH);

    /*let's read in the middle of the SIO_C cycle :*/
    /*read the MSB not read yet :*/
    if(digitalRead(SIO_D) != LOW) {
      *m_data = 0x01 | (*m_data << 1);
    } else {
      *m_data = 0xFE & (*m_data << 1) ;
    }

    delayMicroseconds(SIO_CLKDELAY);
    digitalWrite(SIO_C, LOW);
    delayMicroseconds(SIO_CLKDELAY);
  }

  /*eight bits have been read, let's deal with the ninth Don't-care one*/
  /*the master is responsible for driver SIO_D at logical 1 during the NA bit.*/
  pinMode(SIO_D, OUTPUT);
  digitalWrite(SIO_D, HIGH);

  delayMicroseconds(SIO_CLKDELAY);
  digitalWrite(SIO_C, HIGH);
  delayMicroseconds(SIO_CLKDELAY);

  /*there is something/nothing to read to check the transmission ???*/
  digitalWrite(SIO_C, LOW);
  delayMicroseconds(SIO_CLKDELAY);

  /*let's clean that up : reset as usual as if we had written...*/
  pinMode(SIO_D, OUTPUT);
  digitalWrite(SIO_D, LOW);

  return 1;
}

#endif  // _SCCB_H_
