#include "ht16k33_7seg.h"
extern I2C_HandleTypeDef hi2c1;

const uint8_t encoding[] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111  // 9
};

// Display buffer for 4 digits
static uint16_t displayBuf[4];

// Function to write a command to the HT16K33 over I2C
void ht16k33_write_cmd(uint8_t cmd) {
    HAL_I2C_Mem_Write(&hi2c1, HT16K33_ADDR, cmd, 1, NULL, 0, HAL_MAX_DELAY);
}

// Function to write the display buffer to the HT16K33 over I2C
void ht16k33_write_display_buffer(uint16_t *buf) {
    HAL_I2C_Mem_Write(&hi2c1, HT16K33_ADDR, 0x00, 1, (uint8_t *)buf, 8, HAL_MAX_DELAY);
}

// Function to display a signed integer on the 7-segment display
void ht16k33_display_int(int16_t val) {
    // Check if value is negative
    uint8_t negative = 0;
    if (val < 0) {
        negative = 1;
        val = -val;
    }

    // Convert value to array of digits
    uint8_t digits[3];
    digits[0] = val / 100;
    digits[1] = (val / 10) % 10;
    digits[2] = val % 10;

    // Set the first digit to minus sign
    if (negative) {
        displayBuf[0] = encoding[10];
    } else {
        displayBuf[0] = encoding[digits[0]];
    }

    // Set the remaining digits
    displayBuf[1] = encoding[digits[1]];
    displayBuf[2] = encoding[digits[2]];
    displayBuf[3] = 0;  // Turn off the fourth digit to remove trailing zero

    // Send the display buffer to the HT16K33 over I2C
    ht16k33_write_display_buffer(displayBuf);
}
