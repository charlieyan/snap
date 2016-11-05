#include <SPI.h>
#include <Wire.h>

#include "ov2640_regs.h"

#ifndef OV2640_H
#define OV2640_H

struct regval_list {
  byte reg;
  byte val;
};

/* Supported resolutions */
enum ov2640_width {
  W_QCIF  = 176,
  W_QVGA  = 320,
  W_CIF = 352,
  W_VGA = 640,
  W_SVGA  = 800,
  W_XGA = 1024,
  W_SXGA  = 1280,
  W_UXGA  = 1600,
};

enum ov2640_height {
  H_QCIF  = 144,
  H_QVGA  = 240,
  H_CIF = 288,
  H_VGA = 480,
  H_SVGA  = 600,
  H_XGA = 768,
  H_SXGA  = 1024,
  H_UXGA  = 1200,
};

struct ov2640_win_size {
  char                      *name;
  enum ov2640_width         width;
  enum ov2640_height        height;
  const struct regval_list  *regs;
};

#define ENDMARKER { 0xff, 0xff }
#define OV2640_SIZE(n, w, h, r) \
  {.name = n, .width = w , .height = h, .regs = r }

#define PER_SIZE_REG_SEQ(x, y, v_div, h_div, pclk_div)  \
  { CTRLI, CTRLI_LP_DP | CTRLI_V_DIV_SET(v_div) | \
     CTRLI_H_DIV_SET(h_div)},   \
  { ZMOW, ZMOW_OUTW_SET(x) },     \
  { ZMOH, ZMOH_OUTH_SET(y) },     \
  { ZMHH, ZMHH_OUTW_SET(x) | ZMHH_OUTH_SET(y) },  \
  { R_DVP_SP, pclk_div },       \
  { RESET, 0x00}

static const struct regval_list ov2640_qcif_regs[] = {
  PER_SIZE_REG_SEQ(W_QCIF, H_QCIF, 3, 3, 4),
  ENDMARKER,
};

static const struct regval_list ov2640_qvga_regs[] = {
  PER_SIZE_REG_SEQ(W_QVGA, H_QVGA, 2, 2, 4),
  ENDMARKER,
};

static const struct regval_list ov2640_cif_regs[] = {
  PER_SIZE_REG_SEQ(W_CIF, H_CIF, 2, 2, 8),
  ENDMARKER,
};

static const struct regval_list ov2640_vga_regs[] = {
  PER_SIZE_REG_SEQ(W_VGA, H_VGA, 0, 0, 2),
  ENDMARKER,
};

static const struct regval_list ov2640_svga_regs[] = {
  PER_SIZE_REG_SEQ(W_SVGA, H_SVGA, 1, 1, 2),
  ENDMARKER,
};

static const struct regval_list ov2640_xga_regs[] = {
  PER_SIZE_REG_SEQ(W_XGA, H_XGA, 0, 0, 2),
  { CTRLI,    0x00},
  ENDMARKER,
};

static const struct regval_list ov2640_sxga_regs[] = {
  PER_SIZE_REG_SEQ(W_SXGA, H_SXGA, 0, 0, 2),
  { CTRLI,    0x00},
  { R_DVP_SP, 2 | R_DVP_SP_AUTO_MODE },
  ENDMARKER,
};

static const struct regval_list ov2640_uxga_regs[] = {
  PER_SIZE_REG_SEQ(W_UXGA, H_UXGA, 0, 0, 0),
  { CTRLI,    0x00},
  { R_DVP_SP, 0 | R_DVP_SP_AUTO_MODE },
  ENDMARKER,
};

static const struct ov2640_win_size ov2640_supported_win_sizes[] = {
  OV2640_SIZE("QCIF", W_QCIF, H_QCIF, ov2640_qcif_regs),
  OV2640_SIZE("QVGA", W_QVGA, H_QVGA, ov2640_qvga_regs),
  OV2640_SIZE("CIF",  W_CIF,  H_CIF,  ov2640_cif_regs),
  OV2640_SIZE("VGA",  W_VGA,  H_VGA,  ov2640_vga_regs),
  OV2640_SIZE("SVGA", W_SVGA, H_SVGA, ov2640_svga_regs),
  OV2640_SIZE("XGA",  W_XGA,  H_XGA,  ov2640_xga_regs),
  OV2640_SIZE("SXGA", W_SXGA, H_SXGA, ov2640_sxga_regs),
  OV2640_SIZE("UXGA", W_UXGA, H_UXGA, ov2640_uxga_regs),
};

int32_t sccb_read_reg(uint8_t addr, uint8_t reg, uint8_t *value) {
  systime_t timeout = MS2ST(4);
  uint8_t txbuf[1] = {reg};
  uint8_t rxbuf[1];

  i2cAcquireBus(&I2CD1);
  msg_t status = i2cMasterTransmitTimeout(&I2CD1, addr, txbuf, 1, rxbuf, 1, timeout);
  i2cReleaseBus(&I2CD1);

  if (status != RDY_OK){
    return -1;
  }

  *value = rxbuf[0];

  return 0;
}

int32_t sccb_write_reg(uint8_t addr, uint8_t reg, uint8_t value) {
  systime_t timeout = MS2ST(4);
  uint8_t txbuf[2] = {reg, value};
  uint8_t rxbuf[1];

  i2cAcquireBus(&I2CD1);
  msg_t status = i2cMasterTransmitTimeout(&I2CD1, addr, txbuf, 2, rxbuf, 0, timeout);
  i2cReleaseBus(&I2CD1);

  if (status != RDY_OK){
    return -1;
  }

  return 0;
}

class Ov2640 {
  public:
    Ov2640();
};

#endif
