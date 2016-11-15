#ifndef CAMERA_H
#define CAMERA_H

#include <Arduino.h>
#include "Sccb.h"

class Camera {
  public:
    Camera(
      Sccb* sccb,
      int reset_b,      // teensy -> camera
      int pwd_n,        // teensy -> camera

      int href_pin,     // camera -> teensy
      int pclk_pin,     // camera -> teensy
      int v_sync_pin,   // camera -> teensy
      int pixel_pins[8] // camera -> teensy
    );
  private:
    Sccb* _sccb;
    int _reset_b;
    int _pwd_n;
    int _href_pin;
    int _pclk_pin;
    int _v_sync_pin;
};
#endif

