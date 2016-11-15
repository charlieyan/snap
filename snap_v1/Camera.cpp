#include "Camera.h"

Camera::Camera(
  Sccb* sccb,
  int reset_b,      // teensy -> camera
  int pwd_n,        // teensy -> camera
  int href_pin,     // camera -> teensy
  int pclk_pin,     // camera -> teensy
  int v_sync_pin,   // camera -> teensy
  int pixel_pins[8] // camera -> teensy, only 8 bits on the OV2640 breakout
) {
  _sccb = sccb;
  _reset_b = reset_b; pinMode(_reset_b, OUTPUT);
  _pwd_n = pwd_n; pinMode(_pwd_n, OUTPUT);

  _href_pin = href_pin; pinMode(_href_pin, INPUT);
  _pclk_pin = pclk_pin; pinMode(_pclk_pin, INPUT);
  _v_sync_pin = v_sync_pin; pinMode(_v_sync_pin, INPUT);
}



