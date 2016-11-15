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

/* software reset various components */
#define RESET                   0xE0 /* Reset */
#define RESET_MICROC            0x40
#define RESET_SCCB              0x20
#define RESET_JPEG              0x10
#define RESET_DVP               0x04
#define RESET_IPU               0x02
#define RESET_CIF               0x01

#define MS_SP                   0xF0 /* SCCB Master Speed */
#define SS_ID                   0xF7 /* SCCB Slave ID */

/* SS_CTRL default hex is 01 = enable sensor pass through access */
#define SS_CTRL                 0xF8 /* SCCB Slave Control */
#define SS_CTRL_ADD_AUTO_INC    0x20 /* Bit[5] auto-increase enable */
#define SS_CTRL_SCCB_EN         0x08 /* Bit[3] sccb enable */
#define SS_CTRL_DELAY_CLK       0x04 /* Bit[2] delay sccb master clock */
#define SS_CTRL_ACC_EN          0x02 /* Bit[1] enable sccb master access */
#define SS_CTRL_SEN_PASS_THR    0x01 /* Bit[0] enable sensor pass through access */

/* there is onboard microcontroller to do anything you want, ignore this for now */
#define MC_BIST                 0xF9 /* Microcontroller misc register */
#define MC_BIST_RESET           0x80 /* Microcontroller Reset */
#define MC_BIST_BOOT_ROM_SEL    0x40
#define MC_BIST_12KB_SEL        0x20
#define MC_BIST_12KB_MASK       0x30
#define MC_BIST_512KB_SEL       0x08
#define MC_BIST_512KB_MASK      0x0C
#define MC_BIST_BUSY_BIT_R      0x02
#define MC_BIST_MC_RES_ONE_SH_W 0x02
#define MC_BIST_LAUNCH          0x01

/* BANK_SEL_SENS, 0x01, register addresses */
#define GAIN                    0x00 /* AGC - Gain control gain setting, Gain =(Bit[7]+1) x (Bit[6]+1) x (Bit[5]+1) x (Bit[4]+1) x (1+Bit[3:0]/16) */

#define COM1                    0x03 /* Common control 1 */
#define COM1_1_DUMMY_FR         0x40
#define COM1_3_DUMMY_FR         0x80
#define COM1_7_DUMMY_FR         0xC0
#define COM1_VWIN_LSB_UXGA      0x0F
#define COM1_VWIN_LSB_SVGA      0x0A
#define COM1_VWIN_LSB_CIF       0x06

#define REG04                   0x04 /* Register 04 */
#define REG04_HFLIP_IMG         0x80 /* Horizontal mirror image ON/OFF */
#define REG04_VFLIP_IMG         0x40 /* Vertical flip image ON/OFF */
#define REG04_VREF_EN           0x10
#define REG04_HREF_EN           0x08
#define REG04_AEC_SET(x)        VAL_SET(x, 0x3, 0, 0) /* AEC is automatic exposure control, spread throughout many regs */

#define REG08                   0x08 /* Frame Exposure One-pin Control Pre-charge Row Num ??? */

#define COM2                    0x09 /* Common control 2 */
#define COM2_SOFT_SLEEP_MODE    0x10 /* Soft sleep mode */
#define COM2_OCAP_Nx_SET(N)     (((N) - 1) & 0x03) /* Output drive capability: N = [1x .. 4x] */

#define PID                     0x0A /* Product ID Number MSB, info */
#define VER                     0x0B /* Product ID Number LSB, info */

#define COM3                    0x0C /* Common control 3 */
#define COM3_BAND_50H           0x04 /* 0 For Banding at 60H */
#define COM3_BAND_AUTO          0x02 /* Auto Banding */
#define COM3_SING_FR_SNAPSH     0x01 /* 0 For enable live video output after the snapshot sequence interesting... CHARLIE */

#define COM4                    0x0D

#define AEC                     0x10 /* AEC[9:2] Exposure Value */

#define CLKRC                   0x11 /* Internal clock */
#define CLKRC_EN                0x80 /* Turn on internal freq doubler */
#define CLKRC_DIV_SET(x)        (((x) - 1) & 0x1F) /* CLK = XVCLK/(x) */

#define COM7                    0x12 /* Common control 7 */
#define COM7_SRST               0x80 /* important! Initiates system reset. All registers are set to factory default values after which the chip resumes normal operation */
#define COM7_RES_UXGA           0x00 /* Resolution selectors for UXGA */
#define COM7_RES_CIF            0x20 /* CIF */
#define COM7_RES_SVGA           0x40 /* SVGA */
#define COM7_ZOOM_EN            0x04 /* Enable Zoom mode */
#define COM7_COLOR_BAR_TEST     0x02 /* Enable Color Bar Test Pattern */

#define COM8                    0x13 /* Common control 8 */
#define COM8_DEF                0xC0 /* Banding filter ON/OFF */
#define COM8_AGC_EN             0x04 /* AGC Auto/Manual control selection */
#define COM8_BNDF_EN            0x20 /* Banding filter ON/OFF */
#define COM8_AEC_EN             0x01 /* Auto/Manual Exposure control */

#define COM9                    0x14 /* Common control 9: Automatic gain ceiling - maximum AGC value [7:5]*/
#define COM9_AGC_GAIN_2x        0x00 /* 000 :   2x */
#define COM9_AGC_GAIN_4x        0x20 /* 001 :   4x */
#define COM9_AGC_GAIN_8x        0x40 /* 010 :   8x */
#define COM9_AGC_GAIN_16x       0x60 /* 011 :  16x */
#define COM9_AGC_GAIN_32x       0x80 /* 100 :  32x */
#define COM9_AGC_GAIN_64x       0xA0 /* 101 :  64x */
#define COM9_AGC_GAIN_128x      0xC0 /* 110 : 128x */

#define COM10                   0x15 /* Common control 10 */
#define COM10_PCLK_HREF         0x20 /* PCLK output qualified by HREF */
#define COM10_PCLK_RISE         0x10 /* Data is updated at the rising edge of PCLK (user can latch data at the next, falling edge of PCLK). 0 otherwise. */
#define COM10_HREF_INV          0x08 /* Invert HREF polarity: HREF negative for valid data*/
#define COM10_VSINC_INV         0x02 /* Invert VSYNC polarity */

#define HREFST                  0x17 /* horizontal window start msb 8 bits, 3 lsbs in REG32[2:0] */
#define HREFEND                 0x18 /* horizontal window end 8 bits, 2 lsbs in COM1[1:0] */
#define VSTRT                   0x19 /* vertical window line start MSB 8 bits, 2 lsbs in COM1[1:0] */
#define VEND                    0x1A /* vertical window line end MSB 8 bits, 2 lsbs in COM1[3:2] */

#define AEW                     0x24 /* luminance signal high range for AEC / AGC */
#define AEB                     0x25 /* luminance signal low range AEC / AGC */
#define VV                      0x26 /* fast mode large step range threshold */
#define VV_HIGH_TH_SET(x)       VAL_SET(x, 0xF, 0, 4)
#define VV_LOW_TH_SET(x)        VAL_SET(x, 0xF, 0, 0)

#define REG2A                   0x2A /* 7:4 = line interval adjust value 4 msbs, 3:2 = hsync msb, 1:0 = hsync msb */
#define FRARL                   0x2B /* line interval adjustment value lsb 8 bits, adjust frame rate. each lsb slows down frame rate */

#define ADDVSL                  0x2D /* VSYNC pulse width LSB 8 bits */
#define ADDVSH                  0x2E /* VSYNC pulse width MSB 8 bits */

#define YAVG                    0x2F /* luminance average, audo-updates and calculated from B/Gb/Gr/R channel average */

#define HSDY                    0x30 /* HSYNC position and width, start point LSB 8 bits, LSB 2 in REG2A[1:0] define HSYNC start position */
#define HEDY                    0x31 /* HSYNC position and width, end point LSB 8 bits, REG2A[3:2] define hsync end position, default is 30 */

#define REG32                   0x32 /* Common control 32 7:6 = pixel clock divide 0, 5:3 = HREFST 3 LSB, 2:0 = HREFEND 3 LSB */

#define ARCOM2                  0x34 /* Bit[2] is zoom window horizontal start point ??, default is 0x20  */

#define REG45                   0x45 /* leftovers for other registers: 7:6 = AGC[9:8], AGC highest gain control, 5:0 = AEC[15:10] */

#define FLL                     0x46 /* Frame Length Adjustment LSBs, each bit adds 1 horizontal line timing in frame */
#define FLH                     0x47 /* Frame Length Adjustment MSBs, each bit adds 256 horizontal lines timing in frame */

#define COM19                   0x48 /* Bit[1:0] zoom mode vertical window start point 2 lsbs */

#define ZOOMS                   0x49 /* Zoom Mode Vertical Window Start Point 8 MSB */

#define COM22                   0x4B /* Common Control 22: 7:0 is flash light control CHARLIE TRY THIS */

#define COM25                   0x4E /* Common Control 25: leftovers: 7:6 is 50 Hz 2 MSB, 5:4 is 60 Hz 2 MSBs */
#define BD50                    0x4F /* 50 Hz Banding AEC 8 LSBs */
#define BD60                    0x50 /* 60 Hz banding AEC 8 LSBs */

#define REG5D                   0x5D /* these four are the 32 bits of AVGsel 16-zone avg weight option? */
#define REG5E                   0x5E
#define REG5F                   0x5F
#define REG60                   0x60

#define HISTO_LOW               0x61 /* Histo Algorithm Low Level */  
#define HISTO_HIGH              0x62 /* Histo Algorithm High Level */

#endif
