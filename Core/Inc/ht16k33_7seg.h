#ifndef HT16K33_7SEG_H
#define HT16K33_7SEG_H

#include "stm32f1xx_hal.h"

// HT16K33 I2C address
#define HT16K33_ADDR 0x70

// HT16K33 command codes
#define HT16K33_CMD_OSCILLATOR_ON      0x21
#define HT16K33_CMD_DISPLAY_ON         0x81
#define HT16K33_DISPLAY_BRIGHTNESS_MAX 0x0F




// Function prototypes
void ht16k33_write_cmd(uint8_t cmd);
void ht16k33_write_display_buffer(uint16_t *buf);
void ht16k33_display_int(int16_t val);

#endif /* HT16K33_7SEG_H */
