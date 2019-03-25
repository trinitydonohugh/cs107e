#ifndef barometer_H
#define barometer_H

/*
     barometer-AN4682.pdf
		p8
 */

// register addresses
enum regAddr
    {
      REF_P_XL                = 0x08,
      REF_P_L                 = 0x09,
      REF_P_H                 = 0x0A,

      WHO_AM_I                = 0x0F,

      RES_CONF                = 0x10,

      CTRL_REG1               = 0x20,
      CTRL_REG2               = 0x21,
      CTRL_REG3               = 0x22,
      CTRL_REG4               = 0x23, // 25H

      STATUS_REG              = 0x27,

      PRESS_OUT_XL            = 0x28,
      PRESS_OUT_L             = 0x29,
      PRESS_OUT_H             = 0x2A,

      TEMP_OUT_L              = 0x2B,
      TEMP_OUT_H              = 0x2C,

      FIFO_CTRL               = 0x2E, // 25H
      FIFO_STATUS             = 0x2F, // 25H

      AMP_CTRL                = 0x30, // 331AP

      RPDS_L                  = 0x39, // 25H
      RPDS_H                  = 0x3A, // 25H

      DELTA_PRESS_XL          = 0x3C, // 331AP
      DELTA_PRESS_L           = 0x3D, // 331AP
      DELTA_PRESS_H           = 0x3E, // 331AP


      // dummy addresses for registers in different locations on different devices;
      // the library translates these based on device type
      // value with sign flipped is used as index into translated_regs array

      INTERRUPT_CFG    = -1,
      INT_SOURCE       = -2,
      THS_P_L          = -3,
      THS_P_H          = -4,
      // update dummy_reg_count if registers are added here!


      // device-specific register addresses

      LPS331AP_INTERRUPT_CFG  = 0x23,
      LPS331AP_INT_SOURCE     = 0x24,
      LPS331AP_THS_P_L        = 0x25,
      LPS331AP_THS_P_H        = 0x26,

      LPS25H_INTERRUPT_CFG    = 0x24,
      LPS25H_INT_SOURCE       = 0x25,
      LPS25H_THS_P_L          = 0x30,
      LPS25H_THS_P_H          = 0x31,
    };

void barometer_init();

void barometer_write_reg(unsigned char reg, unsigned char v);
unsigned barometer_read_reg(unsigned char reg);

unsigned barometer_get_whoami();

unsigned int readPressureRaw();
float readPressureInchesMillibars();

#endif
