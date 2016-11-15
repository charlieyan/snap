#ifndef OV2640_H
#define OV2640_H

#include <SPI.h>
#include <Wire.h>

#include "ov2640_regs.h"
#include "ov2640_sizes.h"

class Ov2640 {
  public:
    explicit Ov2640(int sccb_sioc_pin, int sccb_siod_pin);
};

#endif
