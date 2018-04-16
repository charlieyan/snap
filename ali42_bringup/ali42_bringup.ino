// work log here
// i2c_scan says that ali42 exists at addr 0x21 or 33
// tried talking to the device over i2c using Wire.read() but got 0xFF all the time
// so went back to sccb with bitbang_attempt_4, with pull up 2.2k x2 on sck but not sda still worked
// it was able to talk to the device at address of 33, MAKE SURE SCL AND SDA ARE NOT SHARING A LINE
/*
 * some output:
 * r0x00
write_sensor_addr: 42
read_sensor_addr: 43
Got FF
r0x01
write_sensor_addr: 42
read_sensor_addr: 43
Got 80
r0x02
write_sensor_addr: 42
read_sensor_addr: 43
Got 8C
r0x03
write_sensor_addr: 42
read_sensor_addr: 43
Got 0
r0x04
write_sensor_addr: 42
read_sensor_addr: 43
Got 1
r0x05
write_sensor_addr: 42
read_sensor_addr: 43
Got 78
 */
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
// OV7670 seems to just have 1 register set, not like OV2640
// 1E bit[5] is mirror image or not, try W command on changing this
//r0x1E
//write_sensor_addr: 42
//read_sensor_addr: 43
//Got 1
// if we want to flip up [5], and it is 1 now [0] is up
// so 33 is the dec value to set, 0x21 is the hex value
//w0x1E,0x21
//sub_address_int 30
//data_intr 33
//write_sensor_addr: 42
//found good one!
//r0x1E
//write_sensor_addr: 42
//read_sensor_addr: 43
//Got 21
// SUCCESS!!!!!
//w0x1E,0x01
//sub_address_int 30
//data_intr 1
//write_sensor_addr: 42
//found good one!
//r0x1E
//write_sensor_addr: 42
//read_sensor_addr: 43
//Got 1
// flipped it back correctly

// so now, we need to hoook up the PINX and PORTD/C/B macros for the teensy, and hook them up to ali42 correctly
// we can test around with LEDs and the #define macros used in the other code
// seems like both desaster and the ARduinoCamera code uses this
// apparently, there is a mapping from PORTs (multi bits) to PINS (one bit each)
// and the teensy is different than AVR in the firmware on this too

/*
PIXEL HOOKUPS
PORTD0 is pin 2, is D0, yellow wire
PORTD1 is pin 14, is D1, orange wire
PORTD2 is pin 7, is D2, red wire
PORTD3 is pin 8, is D3, brown wire
PORTD4 is pin 6, is D4, white wire
PORTD5 is pin 20, is D5, yellow wire
PORTD6 is pin 21, is D6, green wire
PORTD7 is pin 5, is D7, white wire
*/

// VSYNC line never ticks up, stays at negative
// suspect that is because COM10,
// register 0x15 on Ov7670 is set such
// check over SCCB the register value
// so the value of 0x15 is default of 00
// so that means VSYNC does change on falling edge of PCLK, means it is fw right
// turns out wrong pinning, VSYNC was 14 but should be 19, WEN is 15 instead of 19

// WEN is currently 1 when testing it, does that mean it is ACTIVE LOW? Y datasheet confirms

// after some testing, got PIND some YUV or whatever
// nonzero values after setting all those pins to pinMode INPUT
/*
PIND value68
PIND value160
PIND value160
PIND value160
PIND value160
PIND value68
PIND value68
PIND value68
PIND value4
PIND value0
*/

// so then check 0x12 and it is 00 as default, so that means output format is YUV

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

void InitSCCB(void) {
  pinMode(SIO_C, OUTPUT);
  pinMode(SIO_D, OUTPUT);

  digitalWrite(SIO_C, HIGH);
  digitalWrite(SIO_D, HIGH);
  Serial.println("InitSCCB - Port Direction and set high.");
}

void StartSCCB(void) {
//  Serial.println("Starting SCCB...");
  /*put HIGH in order to avoid propagating unknown bus state.*/
  digitalWrite(SIO_D, HIGH);
  delayMicroseconds(SIO_CLKDELAY);
  digitalWrite(SIO_C, HIGH);
  delayMicroseconds(SIO_CLKDELAY);
  digitalWrite(SIO_D, LOW); // documentation: assert SIO_D low while SIO_C high
  delayMicroseconds(SIO_CLKDELAY);
  digitalWrite(SIO_C, LOW);
//  Serial.println("Starting SCCB : done."); 
}

void StopSCCB(void) {
//  Serial.println("Stoping SCCB...");
  digitalWrite(SIO_D,LOW);
  delayMicroseconds(SIO_CLKDELAY);
  digitalWrite(SIO_C,HIGH);
  delayMicroseconds(SIO_CLKDELAY);
  digitalWrite(SIO_D,HIGH);
  delayMicroseconds(SIO_CLKDELAY);
//  Serial.println("Stoping SCCB : done.");  
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
  if(digitalRead(SIO_D) == HIGH)
  {
//    Serial.println("SCCB Write NG");
    tem = 0;
  } else {
    tem = 1;
//    Serial.println("SCCB Write : OK."); 
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

String readString;

void setup() {
  Serial.begin(115200);
  
  if(InitSensor())
    Serial.println("Init : OK");
  else
    Serial.println("Init : NG....");

  // set up supporting pins directions here
  pinMode(VSYNC, INPUT);
  pinMode(WEN, OUTPUT);
  pinMode(RCLK, OUTPUT);
  pinMode(RRST, OUTPUT);
  digitalWriteFast(WEN, HIGH);  // active low, so disables fifo write
  delayMicroseconds(100);

  pinMode(PIXEL0, INPUT);
  pinMode(PIXEL1, INPUT);
  pinMode(PIXEL2, INPUT);
  pinMode(PIXEL3, INPUT);
  pinMode(PIXEL4, INPUT);
  pinMode(PIXEL5, INPUT);
  pinMode(PIXEL6, INPUT);
  pinMode(PIXEL7, INPUT);
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

void loop() {
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
    x = strtol(rest.c_str(),&pEnd,16);
    byte temp;
    ReadSensor(x, &temp);
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


      Serial.print("sub_address_int ");
      Serial.println(sub_address_int);

      Serial.print("data_intr ");
      Serial.println(data_int);

      byte temp = WriteSensor(sub_address_int, data_int);
      if (temp > 0) {
        Serial.println("write success!");
      }
    }
  }
  else if (readString[0] == 't') {
    for (int j = 0; j < 128; ++j) {
      Serial.println(j);
      StartSCCB();
      byte temp = WriteSCCB(j << 1);
      StopSCCB();
      if (temp > 0) {
        Serial.println("found good one!");
      }
      delay(50);
    }
  }
  else if (readString[0] == 'b') {
    Serial.println("beginning camera");
  }
  else if (readString[0] == 'c') {
    Serial.println("capturing?");
    while (digitalReadFast(VSYNC));
    Serial.println("got a 0 VSYNC");
    while (!digitalReadFast(VSYNC));
    Serial.println("got a 1 VSYNC");
    enableWrite();
    while (digitalReadFast(VSYNC));
    Serial.println("got all vsync reads");
    disableWrite();
  }
  else if (readString[0] == 'y') {
    Serial.println("reading pixels?");
    resetReadPointer();
    Serial.println("done resetting read pointer");
    for (int i = 0; i < 480; ++i) {
      for (int j = 0; j < 640; ++j) {
        digitalWriteFast(RCLK, HIGH);
        Serial.print("PIND value");
        Serial.println(PIND);
        digitalWriteFast(RCLK, LOW);
      }
    }
  }

  readString = "";
}
