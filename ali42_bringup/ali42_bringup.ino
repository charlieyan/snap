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
// put hand over the camera, and the values seemed to change

// next up: writing to a txt file the YUV / RGB values or whatever file on microSD
// analyzing that data in post

// tried parsing yuv file, didn't get much
// set 0x12 to 0x06, which means RGB 555 mode + Color bar enable for testing
// set COM15,0x40 default is 0xC0 = 1100,0000, which means 00 to FF, normal RGB output
// to 00 00 00 00, which means 10 to F0 (8 bits), 4:2:2 format

#ifndef __OV7670REG_H
#define __OV7670REG_H

#define REG_GAIN        0x00    /* Gain lower 8 bits (rest in vref) */
#define REG_BLUE        0x01    /* blue gain */
#define REG_RED         0x02    /* red gain */
#define REG_VREF        0x03    /* Pieces of GAIN, VSTART, VSTOP */
#define REG_COM1        0x04    /* Control 1 */
#define COM1_CCIR656    0x40    /* CCIR656 enable */
#define REG_BAVE        0x05    /* U/B Average level */
#define REG_GbAVE       0x06    /* Y/Gb Average level */
#define REG_AECHH       0x07    /* AEC MS 5 bits */
#define REG_RAVE        0x08    /* V/R Average level */
#define REG_COM2        0x09    /* Control 2 */
#define COM2_SSLEEP     0x10    /* Soft sleep mode */
#define REG_PID         0x0a    /* Product ID MSB */
#define REG_VER         0x0b    /* Product ID LSB */
#define REG_COM3        0x0c    /* Control 3 */
#define COM3_SWAP       0x40    /* Byte swap */
#define COM3_SCALEEN    0x08    /* Enable scaling */
#define COM3_DCWEN      0x04    /* Enable downsamp/crop/window */
#define REG_COM4        0x0d    /* Control 4 */
#define REG_COM5        0x0e    /* All "reserved" */
#define REG_COM6        0x0f    /* Control 6 */
#define REG_AECH        0x10    /* More bits of AEC value */
#define REG_CLKRC       0x11    /* Clocl control */
#define CLK_EXT         0x40    /* Use external clock directly */
#define CLK_SCALE       0x3f    /* Mask for internal clock scale */
#define REG_COM7        0x12    /* Control 7 */
#define COM7_RESET      0x80    /* Register reset */
#define COM7_FMT_MASK   0x38
#define COM7_FMT_VGA    0x00
#define COM7_FMT_CIF    0x20    /* CIF format */
#define COM7_FMT_QVGA   0x10    /* QVGA format */
#define COM7_FMT_QCIF   0x08    /* QCIF format */
#define COM7_RGB        0x04    /* bits 0 and 2 - RGB format */
#define COM7_YUV        0x00    /* YUV */
#define COM7_BAYER      0x01    /* Bayer format */
#define COM7_PBAYER     0x05    /* "Processed bayer" */
#define REG_COM8        0x13    /* Control 8 */
#define COM8_FASTAEC    0x80    /* Enable fast AGC/AEC */
#define COM8_AECSTEP    0x40    /* Unlimited AEC step size */
#define COM8_BFILT      0x20    /* Band filter enable */
#define COM8_AGC        0x04    /* Auto gain enable */
#define COM8_AWB        0x02    /* White balance enable */
#define COM8_AEC        0x01    /* Auto exposure enable */
#define REG_COM9        0x14    /* Control 9  - gain ceiling */
#define REG_COM10       0x15    /* Control 10 */
#define COM10_HSYNC     0x40    /* HSYNC instead of HREF */
#define COM10_PCLK_HB   0x20    /* Suppress PCLK on horiz blank */
#define COM10_HREF_REV  0x08    /* Reverse HREF */
#define COM10_VS_LEAD   0x04    /* VSYNC on clock leading edge */
#define COM10_VS_NEG    0x02    /* VSYNC negative */
#define COM10_HS_NEG    0x01    /* HSYNC negative */
#define REG_HSTART      0x17    /* Horiz start high bits */
#define REG_HSTOP       0x18    /* Horiz stop high bits */
#define REG_VSTART      0x19    /* Vert start high bits */
#define REG_VSTOP       0x1a    /* Vert stop high bits */
#define REG_PSHFT       0x1b    /* Pixel delay after HREF */
#define REG_MIDH        0x1c    /* Manuf. ID high */
#define REG_MIDL        0x1d    /* Manuf. ID low */
#define REG_MVFP        0x1e    /* Mirror / vflip */
#define MVFP_MIRROR     0x20    /* Mirror image */
#define MVFP_FLIP       0x10    /* Vertical flip */
#define REG_AEW         0x24    /* AGC upper limit */
#define REG_AEB         0x25    /* AGC lower limit */
#define REG_VPT         0x26    /* AGC/AEC fast mode op region */
#define REG_HSYST       0x30    /* HSYNC rising edge delay */
#define REG_HSYEN       0x31    /* HSYNC falling edge delay */
#define REG_HREF        0x32    /* HREF pieces */
#define REG_TSLB        0x3a    /* lots of stuff */
#define TSLB_YLAST      0x04    /* UYVY or VYUY - see com13 */
#define REG_COM11       0x3b    /* Control 11 */
#define COM11_NIGHT     0x80    /* NIght mode enable */
#define COM11_NMFR      0x60    /* Two bit NM frame rate */
#define COM11_HZAUTO    0x10    /* Auto detect 50/60 Hz */
#define COM11_50HZ      0x08    /* Manual 50Hz select */
#define COM11_EXP       0x02
#define REG_COM12       0x3c    /* Control 12 */
#define COM12_HREF      0x80    /* HREF always */
#define REG_COM13       0x3d    /* Control 13 */
#define COM13_GAMMA     0x80    /* Gamma enable */
#define COM13_UVSAT     0x40    /* UV saturation auto adjustment */
#define COM13_UVSWAP    0x01    /* V before U - w/TSLB */
#define REG_COM14       0x3e    /* Control 14 */
#define COM14_DCWEN     0x10    /* DCW/PCLK-scale enable */
#define REG_EDGE        0x3f    /* Edge enhancement factor */
#define REG_COM15       0x40    /* Control 15 */
#define COM15_R10F0     0x00    /* Data range 10 to F0 */
#define COM15_R01FE     0x80    /*            01 to FE */
#define COM15_R00FF     0xc0    /*            00 to FF */
#define COM15_RGB565    0x10    /* RGB565 output */
#define COM15_RGB555    0x30    /* RGB555 output */
#define REG_COM16       0x41    /* Control 16 */
#define COM16_AWBGAIN   0x08    /* AWB gain enable */
#define REG_COM17       0x42    /* Control 17 */
#define COM17_AECWIN    0xc0    /* AEC window - must match COM4 */
#define COM17_CBAR      0x08    /* DSP Color bar */
#define REG_CMATRIX_BASE 0x4f
#define CMATRIX_LEN 6
#define REG_CMATRIX_SIGN 0x58
#define REG_BRIGHT      0x55    /* Brightness */
#define REG_CONTRAS     0x56    /* Contrast control */
#define REG_GFIX        0x69    /* Fix gain control */
#define REG_REG76       0x76    /* OV's name */
#define R76_BLKPCOR     0x80    /* Black pixel correction enable */
#define R76_WHTPCOR     0x40    /* White pixel correction enable */
#define REG_RGB444      0x8c    /* RGB 444 control */
#define R444_ENABLE     0x02    /* Turn on RGB444, overrides 5x5 */
#define R444_RGBX       0x01    /* Empty nibble at end */
#define REG_HAECC1      0x9f    /* Hist AEC/AGC control 1 */
#define REG_HAECC2      0xa0    /* Hist AEC/AGC control 2 */
#define REG_BD50MAX     0xa5    /* 50hz banding step limit */
#define REG_HAECC3      0xa6    /* Hist AEC/AGC control 3 */
#define REG_HAECC4      0xa7    /* Hist AEC/AGC control 4 */
#define REG_HAECC5      0xa8    /* Hist AEC/AGC control 5 */
#define REG_HAECC6      0xa9    /* Hist AEC/AGC control 6 */
#define REG_HAECC7      0xaa    /* Hist AEC/AGC control 7 */
#define REG_BD60MAX     0xab    /* 60hz banding step limit */

#endif

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

#include <SPI.h>
#include <SD.h>

File read_frame_file;
String readString;

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

//  if (!SD.begin(10)) {
//    Serial.println("SD begin(10) failed!");
////    while (1);
//  }
//  Serial.println("SD begin(10) done.");
}

void printBinary(byte inByte)
{
  for (int b = 7; b >= 0; b--)
  {
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

void set_reg(String sub_address, String data) {
  char* sub_address_c;
  long int sub_address_int = strtol(sub_address.c_str(),&sub_address_c,16);

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

    set_reg_2(REG_COM7, 0x80); // reset to default values
    set_reg_2(REG_CLKRC, 0x81);  // jim: was 0x80

    set_reg_2(REG_COM11, 0x0A);
    set_reg_2(REG_TSLB, 0x04);
    set_reg_2(REG_TSLB, 0x04);
    set_reg_2(REG_COM7, 0x14); // output format: rgb

    set_reg_2(REG_RGB444, 0x00); // disable RGB444
    set_reg_2(REG_COM15, 0xD0); // RGB565

    // not even sure what all these do, gonna
    // check the oscilloscope and go from there...
    set_reg_2(REG_HSTART, 0x16);
    set_reg_2(REG_HSTOP, 0x04);
    set_reg_2(REG_HREF, 0x24);
    set_reg_2(REG_VSTART, 0x02);
    set_reg_2(REG_VSTOP, 0x7a);
    set_reg_2(REG_VREF, 0x0a);
    set_reg_2(REG_COM10, 0x02);
    set_reg_2(REG_COM3, 0x04);
    set_reg_2(REG_MVFP, 0x3f);

    // 160x120, i think
    //set_reg_2(REG_COM14, 0x1a); // divide by 4
    //set_reg_2(0x72, 0x22); // downsample by 4
    //set_reg_2(0x73, 0xf2); // divide by 4

    // 320x240:
    set_reg_2(REG_COM14, 0x1A);  // jim: 0x19
    set_reg_2(0x72, 0x11);
    set_reg_2(0x73, 0xf2);

    // test pattern
    set_reg_2(0x70, 0xf0);
    set_reg_2(0x71, 0xf0);

    // COLOR SETTING
    set_reg_2(0x4f, 0x80);
    set_reg_2(0x50, 0x80);
    set_reg_2(0x51, 0x00);
    set_reg_2(0x52, 0x22);
    set_reg_2(0x53, 0x5e);
    set_reg_2(0x54, 0x80);
    set_reg_2(0x56, 0x40);
    set_reg_2(0x58, 0x9e);
    set_reg_2(0x59, 0x88);
    set_reg_2(0x5a, 0x88);
    set_reg_2(0x5b, 0x44);
    set_reg_2(0x5c, 0x67);
    set_reg_2(0x5d, 0x49);
    set_reg_2(0x5e, 0x0e);

    set_reg_2(0x69, 0xFF);

    set_reg_2(0x6a, 0x40);
    set_reg_2(0x6b, 0x0a);
    set_reg_2(0x6c, 0x0a);
    set_reg_2(0x6d, 0x55);
    set_reg_2(0x6e, 0x11);
    set_reg_2(0x6f, 0x9f);

    Serial.println("done");
    /*
    ov7670_set(REG_COM11, 0x0A);
    ov7670_set(REG_TSLB, 0x04);
    ov7670_set(REG_TSLB, 0x04);
    ov7670_set(REG_COM7, 0x04); // output format: rgb

    ov7670_set(REG_RGB444, 0x00); // disable RGB444
    ov7670_set(REG_COM15, 0xD0); // RGB565

    // not even sure what all these do, gonna
    // check the oscilloscope and go from there...
    ov7670_set(REG_HSTART, 0x16);
    ov7670_set(REG_HSTOP, 0x04);
    ov7670_set(REG_HREF, 0x24);
    ov7670_set(REG_VSTART, 0x02);
    ov7670_set(REG_VSTOP, 0x7a);
    ov7670_set(REG_VREF, 0x0a);
    ov7670_set(REG_COM10, 0x02);
    ov7670_set(REG_COM3, 0x04);
    ov7670_set(REG_MVFP, 0x3f);

    // 160x120, i think
    //ov7670_set(REG_COM14, 0x1a); // divide by 4
    //ov7670_set(0x72, 0x22); // downsample by 4
    //ov7670_set(0x73, 0xf2); // divide by 4

    // 320x240:
    ov7670_set(REG_COM14, 0x19);
    ov7670_set(0x72, 0x11);
    ov7670_set(0x73, 0xf1);

    // test pattern
    //ov7670_set(0x70, 0xf0);
    //ov7670_set(0x71, 0xf0);

    // COLOR SETTING
    ov7670_set(0x4f, 0x80);
    ov7670_set(0x50, 0x80);
    ov7670_set(0x51, 0x00);
    ov7670_set(0x52, 0x22);
    ov7670_set(0x53, 0x5e);
    ov7670_set(0x54, 0x80);
    ov7670_set(0x56, 0x40);
    ov7670_set(0x58, 0x9e);
    ov7670_set(0x59, 0x88);
    ov7670_set(0x5a, 0x88);
    ov7670_set(0x5b, 0x44);
    ov7670_set(0x5c, 0x67);
    ov7670_set(0x5d, 0x49);
    ov7670_set(0x5e, 0x0e);
    ov7670_set(0x69, 0x00);
    ov7670_set(0x6a, 0x40);
    ov7670_set(0x6b, 0x0a);
    ov7670_set(0x6c, 0x0a);
    ov7670_set(0x6d, 0x55);
    ov7670_set(0x6e, 0x11);
    ov7670_set(0x6f, 0x9f);

    ov7670_set(0xb0, 0x84);
    */
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
    // read_frame_file = SD.open("charlie.txt", FILE_WRITE);
    for (int i = 0; i < 240; ++i) {
      for (int j = 0; j < 320; ++j) {
        Serial.print("PIND value ");

        digitalWriteFast(RCLK, HIGH);
        printBinary(PIND);
        digitalWriteFast(RCLK, LOW);

        digitalWriteFast(RCLK, HIGH);
        printBinary(PIND);
        digitalWriteFast(RCLK, LOW);

        Serial.println("");
      }
    }
    // read_frame_file.close();
    Serial.println("done");
  }

  readString = "";
}
