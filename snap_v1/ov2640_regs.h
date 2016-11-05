
#ifndef OV2640_REGS_H
#define OV2640_REGS_H

/* useful macro */
#define VAL_SET(x, mask, rshift, lshift) ((((x) >> rshift) & mask) << lshift)

#define BANK_SEL              0xFF /* Register Bank Select */
#define BANK_SEL_DSP          0x00 /* Table 12 in the docs */
#define BANK_SEL_SENS         0x01 /* Table 13 in the docs */

/* BANK_SEL_DSP, 0x00, register addresses */
#define R_BYPASS              0x05 /* Bypass DSP */
#define R_BYPASS_USE_DSP      0x00 /* Use the internal DSP */
#define R_BYPASS_DSP_BYPAS    0x01 /* Bypass DSP, sensor out directly */

#define QS                    0x44 /* Quantization Scale Factor, not sure what this does yet */

#define CTRLI                 0x50 /* not sure what this does yet */
#define CTRLI_LP_DP           0x80 /* MSB */
#define CTRLI_ROUND           0x40 /* MSB - 1 */
#define CTRLI_V_DIV_SET(x)    VAL_SET(x, 0x3, 0, 3) /* 5:3 */
#define CTRLI_H_DIV_SET(x)    VAL_SET(x, 0x3, 0, 0) /* 2:0 */

#define HSIZE                 0x51 /* H_SIZE[7:0] (real/4) */
#define HSIZE_SET(x)          VAL_SET(x, 0xFF, 2, 0)

#define VSIZE                 0x52 /* V_SIZE[7:0] (real/4) */s
#define VSIZE_SET(x)          VAL_SET(x, 0xFF, 2, 0)

#define XOFFL                 0x53 /* OFFSET_X[7:0] */
#define XOFFL_SET(x)          VAL_SET(x, 0xFF, 0, 0)

#define YOFFL                 0x54 /* OFFSET_Y[7:0] */
#define YOFFL_SET(x)          VAL_SET(x, 0xFF, 0, 0)

#define VHYX                  0x55
#define VHYX_VSIZE_SET(x)     VAL_SET(x, 0x1, (8+2), 7)
#define VHYX_HSIZE_SET(x)     VAL_SET(x, 0x1, (8+2), 3)
#define VHYX_YOFF_SET(x)      VAL_SET(x, 0x3, 8, 4)
#define VHYX_XOFF_SET(x)      VAL_SET(x, 0x3, 8, 0)

#define DPRP                  0x56

#define TEST                  0x57 /* Horizontal size completion Bit[7]: H_SIZE[9] */ 

#define ZMOW                  0x5A /* OUTW[7:0] (real/4) */
#define ZMOW_OUTW_SET(x)      VAL_SET(x, 0xFF, 2, 0)

#define ZMOH                  0x5B /* Zoom: Out Height OUTH[7:0] (real/4) */
#define ZMOH_OUTH_SET(x)      VAL_SET(x, 0xFF, 2, 0)

#define ZMHH                  0x5C /* Zoom: Speed and H&W completion */
#define ZMHH_ZSPEED_SET(x)    VAL_SET(x, 0x0F, 0, 4)
#define ZMHH_OUTH_SET(x)      VAL_SET(x, 0x1, (8+2), 2)
#define ZMHH_OUTW_SET(x)      VAL_SET(x, 0x3, (8+2), 0)

#define BPADDR                0x7C /* SDE Indirect Register Access: Address */
#define BPDATA                0x7D /* SDE Indirect Register Access: Data */

#define CTRL2                 0x86 /* DSP Module enable 2 */
#define CTRL2_DCW_EN          0x20 /* Bit[5] */
#define CTRL2_SDE_EN          0x10 /* Bit[4] */
#define CTRL2_UV_ADJ_EN       0x08 /* Bit[3] */
#define CTRL2_UV_AVG_EN       0x04 /* Bit[2] */
#define CTRL2_CMX_EN          0x01 /* Bit[0] */

#define CTRL3                 0x87 /* DSP Module enable 3 */
#define CTRL3_BPC_EN          0x80 /* Bit 7 */
#define CTRL3_WPC_EN          0x40 /* Bit 6 */

#define SIZEL                   0x8C /* HSIZE[11], HSIZE[2:0], VSIZE[2:0] */
#define SIZEL_HSIZE8_11_SET(x)  VAL_SET(x, 0x1, 11, 6)
#define SIZEL_HSIZE8_SET(x)     VAL_SET(x, 0x7, 0, 3)
#define SIZEL_VSIZE8_SET(x)     VAL_SET(x, 0x7, 0, 0)

#define HSIZE8                0xC0 /* Image Horizontal Size HSIZE[10:3] */
#define HSIZE8_SET(x)         VAL_SET(x, 0xFF, 3, 0)
#define VSIZE8                0xC1 /* Image Vertical Size VSIZE[10:3] */
#define VSIZE8_SET(x)         VAL_SET(x, 0xFF, 3, 0)

#define CTRL0                 0xC2 /* DSP Module enable 0 */
#define CTRL0_AEC_EN          0x80
#define CTRL0_AEC_SEL         0x40
#define CTRL0_STAT_SEL        0x20
#define CTRL0_VFIRST          0x10
#define CTRL0_YUV422          0x08
#define CTRL0_YUV_EN          0x04
#define CTRL0_RGB_EN          0x02
#define CTRL0_RAW_EN          0x01

#define CTRL1                 0xC3 /* DSP Module enable 1 */
#define CTRL1_CIP             0x80
#define CTRL1_DMY             0x40
#define CTRL1_RAW_GMA         0x20
#define CTRL1_DG              0x10
#define CTRL1_AWB             0x08
#define CTRL1_AWB_GAIN        0x04
#define CTRL1_LENC            0x02
#define CTRL1_PRE             0x01

#define R_DVP_SP              0xD3 /* 7 = auto mode, 6:0 = DVP output speed control, sysclk / 6:0 for YUV0, 2*6:0 for RAW */
#define R_DVP_SP_AUTO_MODE    0x80
#define R_DVP_SP_DVP_MASK     0x3F

/* Image output format select:
 * 6 = Y8 enable for DVP, 
 * 4 = JPEG output (1 = JPEG output)
 * 3:2 is DVP output (00 = YUV422, 01 = RAW10, 10 = RGB565, 11 = RSVD
 * 1 HREF timing select in DVP JPEG output mode (0 = same as sensor, 1 = VSYNC
 * 0 Byte swap enable for DVP 0 = high byte first, 1 = low byte first
 */
#define IMAGE_MODE              0xDA
#define IMAGE_MODE_Y8_DVP_EN    0x40
#define IMAGE_MODE_JPEG_EN      0x10
#define IMAGE_MODE_YUV422       0x00
#define IMAGE_MODE_RGB565       0x08
#define IMAGE_MODE_RAW10        0x04 /* (DVP) */
#define IMAGE_MODE_HREF_VSYNC   0x02
#define IMAGE_MODE_LBYTE_FIRST  0x01

#define RESET                   0xE0 /* Reset */
#define RESET_MICROC            0x40
#define RESET_SCCB              0x20
#define RESET_JPEG              0x10
#define RESET_DVP               0x04
#define RESET_IPU               0x02
#define RESET_CIF               0x01

#define MS_SP                   0xF0 /* SCCB Master Speed */
#define SS_ID                   0xF7 /* SCCB Slave ID */

#define SS_CTRL                 0xF8 /* SCCB Slave Control */
#define SS_CTRL_ADD_AUTO_INC    0x20 /* Bit[5] auto-increase enable */
#define SS_CTRL_SCCB_EN         0x08 /* Bit[3] sccb enable */
#define SS_CTRL_DELAY_CLK       0x04 /* Bit[2] delay sccb master clock */
#define SS_CTRL_ACC_EN          0x02 /* Bit[1] enable sccb master access */
#define SS_CTRL_SEN_PASS_THR    0x01 /* Bit[0] enable sensor pass through access */

#endif
