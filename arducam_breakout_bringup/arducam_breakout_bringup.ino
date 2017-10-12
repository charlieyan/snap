#include <SPI.h>
#include <ArduCAM.h>

#define SIO_C 23
#define SIO_D 22

#define SPI_CS 10 // slave select for SPI

#define FIFO_SIZE 0x07FFFFF
#define FRAMES_NUM 0x06

ArduCAM myCAM(SIO_C, SIO_D, SPI_CS);

String readString;
int start_capture = 0;
uint8_t temp;

void setup()
{
  Serial.begin(115200);
  Serial.println("starting");

  myCAM.InitSCCB();
  int sensor_address = myCAM.ScanSCCB();
  if (sensor_address > -1) {
    Serial.print("assigned sensor_addr_ ");
    Serial.println(sensor_address, HEX);
    myCAM.sensor_addr_ = sensor_address;
  }
  uint8_t vid, pid;
  myCAM.rdSensorReg16_8(OV5642_CHIPID_HIGH, &vid);
  myCAM.rdSensorReg16_8(OV5642_CHIPID_LOW, &pid);
  if ((vid != 0x56) || (pid != 0x42))
    Serial.println("Can't find OV5640 module!");
  else
    Serial.println("OV5640 detected.");
  /* Change to JPEG capture mode 
   * initialize the OV5642 module */
  myCAM.set_format(JPEG);
  myCAM.InitCAM();  // SCCB
  myCAM.OV5642_set_JPEG_size(OV5642_320x240);  // SCCB

  pinMode(SPI_CS, OUTPUT);
  SPI.begin();
  //Check if the ArduCAM SPI bus is OK
  myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
  uint8_t temp = myCAM.read_reg(ARDUCHIP_TEST1);
  if (temp != 0x55) {
    Serial.println("SPI interface Error!");
  }
  myCAM.clear_fifo_flag();
  myCAM.write_reg(ARDUCHIP_FRAMES, 0x00);
  // Bit[2:0]Number of frames to be captured

  temp = myCAM.read_reg(ARDUCHIP_MODE);
  Serial.print("temp ");
  Serial.println(temp, HEX);
}
 
void loop()
{
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
    myCAM.rdSensorReg16_8(x, &temp);
    Serial.print("got ");
    Serial.println(temp, HEX);
  }
  else if (readString[0] == 'w') {
    if (readString.length() == 12) {
      String sub_address = readString.substring(1, 7); // 1, 5 for 1 byte
      long int sub_address_int;
      char* sub_address_c;
      sub_address_int = strtol(sub_address.c_str(),&sub_address_c,16);

      String data = readString.substring(8, 12); // 7, 10 for 1 byte
      long int data_int;
      char* data_c;
      data_int = strtol(data.c_str(),&data_c,16);

      Serial.print("sub_address_int ");
      Serial.println(sub_address_int);
      Serial.print("data_intr ");
      Serial.println(data_int);

      myCAM.wrSensorReg16_8(sub_address_int, data_int);
    }
  }
  else if (readString[0] == 'c') {
    Serial.println("single shoot");
    uint8_t temp;
    myCAM.flush_fifo();
    myCAM.clear_fifo_flag();
    myCAM.start_capture();
    start_capture = 1;
  }

  if (start_capture) {
    myCAM.clear_fifo_flag();
    myCAM.start_capture();
    start_capture = 0;
  }

  if(myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)) {
    Serial.println("Capture Done!");
  }

  readString = "";
}
