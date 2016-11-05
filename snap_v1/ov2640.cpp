#include "ov2640.h"

const struct regval_list OV2640_INIT_REG_VALS[] = {
  {BANK_SEL, BANK_SEL_DSP} /* switch to register bank 00 */
  
};

Ov2640::Ov2640() {
}
