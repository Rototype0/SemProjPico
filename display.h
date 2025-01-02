#pragma once
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "ssd1306_font.h"

// Define the size of the display we have attached. This can vary, make sure you
// have the right size defined or the output will look rather odd!
// Code has been tested on 128x32 and 128x64 OLED displays
#define SSD1306_HEIGHT              64
#define SSD1306_WIDTH               128

#define SSD1306_I2C_ADDR            _u(0x3C)

// 400 is usual, but often these can be overclocked to improve display response.
// Tested at 1000 on both 32 and 84 pixel height devices and it worked.
#define SSD1306_I2C_CLK             400
//#define SSD1306_I2C_CLK             1000


// commands (see datasheet)
#define SSD1306_SET_MEM_MODE        _u(0x20)
#define SSD1306_SET_COL_ADDR        _u(0x21)
#define SSD1306_SET_PAGE_ADDR       _u(0x22)
#define SSD1306_SET_HORIZ_SCROLL    _u(0x26)
#define SSD1306_SET_SCROLL          _u(0x2E)

#define SSD1306_SET_DISP_START_LINE _u(0x40)

#define SSD1306_SET_CONTRAST        _u(0x81)
#define SSD1306_SET_CHARGE_PUMP     _u(0x8D)

#define SSD1306_SET_SEG_REMAP       _u(0xA0)
#define SSD1306_SET_ENTIRE_ON       _u(0xA4)
#define SSD1306_SET_ALL_ON          _u(0xA5)
#define SSD1306_SET_NORM_DISP       _u(0xA6)
#define SSD1306_SET_INV_DISP        _u(0xA7)
#define SSD1306_SET_MUX_RATIO       _u(0xA8)
#define SSD1306_SET_DISP            _u(0xAE)
#define SSD1306_SET_COM_OUT_DIR     _u(0xC0)
#define SSD1306_SET_COM_OUT_DIR_FLIP _u(0xC0)

#define SSD1306_SET_DISP_OFFSET     _u(0xD3)
#define SSD1306_SET_DISP_CLK_DIV    _u(0xD5)
#define SSD1306_SET_PRECHARGE       _u(0xD9)
#define SSD1306_SET_COM_PIN_CFG     _u(0xDA)
#define SSD1306_SET_VCOM_DESEL      _u(0xDB)

#define SSD1306_PAGE_HEIGHT         _u(8)
#define SSD1306_NUM_PAGES           (SSD1306_HEIGHT / SSD1306_PAGE_HEIGHT)
#define SSD1306_BUF_LEN             (SSD1306_NUM_PAGES * SSD1306_WIDTH)

#define SSD1306_WRITE_MODE         _u(0xFE)
#define SSD1306_READ_MODE          _u(0xFF)


struct render_area_t {
    uint8_t start_col;
    uint8_t end_col;
    uint8_t start_page;
    uint8_t end_page;

    int buflen;
};

/**
 *@brief Class for handling i2c display
 */
class display
{
private:
    render_area_t render_area;
    uint8_t buf[SSD1306_BUF_LEN];
    void calc_render_area_buflen();
    void SSD1306_send_cmd(uint8_t cmd);
    void SSD1306_send_cmd_list(uint8_t *buf, int num);
    void SSD1306_send_buf();
    inline int GetFontIndex(uint8_t ch);
public:
    display(/* args */);
    ~display();
    /**
     * @brief Renders current buffer on display
     */
    void render();
    /**
     * @brief clears display buffer
     * @param immediate if true renders cleared display buffer immediately
     */
    void clear(bool immediate=true);
    /**
     * @brief fills display buffer
     * @param immediate if true renders filled display buffer immediately
     */
    void fill(bool immediate=true);
    /**
     * @brief Writes character at specified location
     * @param x x coordinate to write char at
     * @param y y coordinate to write char at
     * @param ch character to write
     * @param immediate render character immediately
     */
    void WriteChar(int16_t x, int16_t y, uint8_t ch, bool immediate=true);
    /**
     * @brief Writes string at specified location
     * @param x x coordinate to write char at
     * @param y y coordinate to write char at
     * @param str string to write
     * @param immediate render string immediately
     */
    void WriteString(int16_t x, int16_t y, char *str, bool immediate=true);
    /**
     * @brief Prints to selected line on display
     * @param line_nr line number between 0 and 7
     * @param immediate render printed line immediately
     * @param format string format like regular printf
     */
    void _printf(uint line_nr, bool immediate, const char* format, ...);
};

/**
 * @brief subclass to pring motor info in one function
 */
class Motor_display : public display
{
public:
/**
 * @brief prints motor information
 * @param dir direction motor is driving
 * @param speed speed of motor
 */
    void print_motor(bool dir, float speed);
};