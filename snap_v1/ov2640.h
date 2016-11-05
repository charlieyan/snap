#ifndef OV2640_H
#define OV2640_H

#include <SPI.h>
#include <Wire.h>

#include "ov2640_regs.h"
#include "ov2640_sizes.h"

class Ov2640 {
  public:
    /* for now, 2-Wire SCCB interface which means only one slave camera device to the teensy */

    /* the master (teensy) must do one of the two master control methods */
    /* 1. the master device must be able to support maintain the data line of the bus in tri-state mode */
    /* 2. drive data line high or low, note the transition there to assert communications with slave */

    
    /* sccb_sioc = SCCB Input Clock */
    /* sccb_siod = SCCB Input Data */
    explicit Ov2640(int sccb_sioc_pin, int sccb_siod_pin);
};

#endif
