#ifndef SCCB_H
#define SCCB_H

struct regval_list { // Register, value list entry
  byte reg;
  byte val;
};

class Sccb {
  public:
    explicit Sccb(int sccb_sioc_pin, int sccb_siod_pin, int device_id);

    int scanForDevices(void);
    bool device_found(void);
    byte readRegister(int device_id, byte address);
    int writeRegister(int device_id, byte address, byte data);
    int writeRegisterArray(int device_id, const struct regval_list *vals);
  private:
    int _device_id;
};
#endif

