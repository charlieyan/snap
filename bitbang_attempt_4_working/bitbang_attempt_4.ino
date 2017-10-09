#define SIO_C 23
#define SIO_D 22
#define M_CLK 21
#define P_CLK 20
#define RESET 0

#define SIO_CLKDELAY 100
#define sensor_addr 48 // 48 in dec

void InitSCCB(void)
{
  pinMode(SIO_C, OUTPUT);
  pinMode(SIO_D, OUTPUT);
  
  pinMode(M_CLK, OUTPUT);
  analogWriteFrequency(M_CLK, 12000000); // 15 MHz
  analogWrite(M_CLK, 128);

  pinMode(P_CLK, INPUT);

  pinMode(RESET, OUTPUT);
  digitalWrite(RESET, HIGH);
  
  digitalWrite(SIO_C, HIGH);
  digitalWrite(SIO_D, HIGH);
  Serial.println("InitSCCB - Port Direction and set high.");
}

void StartSCCB(void)
{
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

void StopSCCB(void)
{
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
  
  /* let's reset the SCCB ! */
//  if (WriteSensor(0xFF, 0x01) == 0) {
//    Serial.println("Problem : Resetting SCCB NG.");
//    return(0);
//  }
  
  return(0x01);
}

String readString;

void setup() {
  Serial.begin(115200);
  
  if(InitSensor())
    Serial.println("Init OV2640 : OK");
  else
    Serial.println("Init OV2640 : NG....");
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
        Serial.println("found good one!");
      }
    }
  }
  else if (readString[0] == 'x') {
    for (int j = 0; j < 2; ++j) {
      StartSCCB();
      byte temp = WriteSCCB(48 << 1);
      StopSCCB();
      if (temp > 0) {
        Serial.println("found good one!");
      }
      delay(50);
    }
  }
  else if (readString[0] == 't') {
    for (int j = 40; j < 50; ++j) {
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
  else if (readString[0] == 'l') {
      Serial.println(43);
      StartSCCB();
      byte temp = WriteSCCB(43 << 1);
      StopSCCB();
      if (temp > 0) {
        Serial.println("found good one!");
      }
  }
  else if (readString[0] == 'n') {
    for (int j = 0; j < 256; j++) {
      Serial.println(j);
      byte temp;
      ReadSensor(j, &temp);
      delay(500);
    }
  }
  else if (readString[0] == 'p') {
    byte temp;
    StartSCCB();
    Serial.println(WriteSCCB(0x60), HEX);
    Serial.println(WriteSCCB(0x1C), HEX);
    StopSCCB();
    StartSCCB();
    Serial.println(WriteSCCB(0x61), HEX);
    Serial.println(ReadSCCB(&temp), HEX);
    StopSCCB();

    Serial.print("temp: ");
    Serial.println(temp);
  }

  readString = "";
}
