#ifndef SCCB_H
#define SCCB_H

struct regval_list { // Register, value list entry
  byte reg;
  byte val;
};

class Sccb {
  public:
    Sccb();
    int scanForDevices(void);
    bool device_found(void);
    byte readRegister(int device_id, byte address);
    int writeRegister(int device_id, byte address, byte data);
    int writeRegisterArray(int device_id, const struct regval_list *vals);
    int _device_id;
};
#endif

