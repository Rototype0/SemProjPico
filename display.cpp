#include "display.h"
void display::calc_render_area_buflen() 
{
    // calculate how long the flattened buffer will be for a render area
    render_area.buflen = (render_area.end_col - render_area.start_col + 1) * (render_area.end_page - render_area.start_page + 1);
}
void display::SSD1306_send_cmd(uint8_t cmd) 
{
    // I2C write process expects a control byte followed by data
    // this "data" can be a command or data to follow up a command
    // Co = 1, D/C = 0 => the driver expects a command
    uint8_t buf[2] = {0x80, cmd};
    i2c_write_blocking(i2c_default, SSD1306_I2C_ADDR, buf, 2, false);
}
void display::SSD1306_send_cmd_list(uint8_t *buf, int num)
{
    for (int i=0;i<num;i++)
        SSD1306_send_cmd(buf[i]);
}
void display::SSD1306_send_buf()
{
    // in horizontal addressing mode, the column address pointer auto-increments
    // and then wraps around to the next page, so we can send the entire frame
    // buffer in one gooooooo!

    // copy our frame buffer into a new buffer because we need to add the control byte
    // to the beginning

    uint8_t *temp_buf = (uint8_t *)malloc(render_area.buflen + 1);

    temp_buf[0] = 0x40;
    memcpy(temp_buf+1, buf, render_area.buflen);

    i2c_write_blocking(i2c_default, SSD1306_I2C_ADDR, temp_buf, render_area.buflen + 1, false);

    free(temp_buf);
}
inline int display::GetFontIndex(uint8_t ch) {
    if (ch >= 32 && ch <=125) 
        return ch -32;
    else
        return  0; // Not got that char so space.
}

display::display(/* args */)
{
    render_area.start_col = 0;
    render_area.end_col = SSD1306_WIDTH - 1;
    render_area.start_page = 0;
    render_area.end_page = SSD1306_NUM_PAGES - 1;
    calc_render_area_buflen();
    i2c_init(i2c_default, SSD1306_I2C_CLK * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Some of these commands are not strictly necessary as the reset
    // process defaults to some of these but they are shown here
    // to demonstrate what the initialization sequence looks like
    // Some configuration values are recommended by the board manufacturer

    uint8_t cmds[] = {
        SSD1306_SET_DISP,               // set display off
        /* memory mapping */
        SSD1306_SET_MEM_MODE,           // set memory address mode 0 = horizontal, 1 = vertical, 2 = page
        0x00,                           // horizontal addressing mode
        /* resolution and layout */
        SSD1306_SET_DISP_START_LINE,    // set display start line to 0
        SSD1306_SET_SEG_REMAP | 0x01,   // set segment re-map, column address 127 is mapped to SEG0
        SSD1306_SET_MUX_RATIO,          // set multiplex ratio
        SSD1306_HEIGHT - 1,             // Display height - 1
        SSD1306_SET_COM_OUT_DIR | 0x08, // set COM (common) output scan direction. Scan from bottom up, COM[N-1] to COM0
        SSD1306_SET_DISP_OFFSET,        // set display offset
        0x00,                           // no offset
        SSD1306_SET_COM_PIN_CFG,        // set COM (common) pins hardware configuration. Board specific magic number.
                                        // 0x02 Works for 128x32, 0x12 Possibly works for 128x64. Other options 0x22, 0x32
#if ((SSD1306_WIDTH == 128) && (SSD1306_HEIGHT == 32))
        0x02,
#elif ((SSD1306_WIDTH == 128) && (SSD1306_HEIGHT == 64))
        0x12,
#else
        0x02,
#endif
        /* timing and driving scheme */
        SSD1306_SET_DISP_CLK_DIV,       // set display clock divide ratio
        0x80,                           // div ratio of 1, standard freq
        SSD1306_SET_PRECHARGE,          // set pre-charge period
        0xF1,                           // Vcc internally generated on our board
        SSD1306_SET_VCOM_DESEL,         // set VCOMH deselect level
        0x30,                           // 0.83xVcc
        /* display */
        SSD1306_SET_CONTRAST,           // set contrast control
        0xFF,
        SSD1306_SET_ENTIRE_ON,          // set entire display on to follow RAM content
        SSD1306_SET_NORM_DISP,           // set normal (not inverted) display
        SSD1306_SET_CHARGE_PUMP,        // set charge pump
        0x14,                           // Vcc internally generated on our board
        SSD1306_SET_SCROLL | 0x00,      // deactivate horizontal scrolling if set. This is necessary as memory writes will corrupt if scrolling was enabled
        SSD1306_SET_DISP | 0x01, // turn display on
        };

    SSD1306_send_cmd_list(cmds, count_of(cmds));
    clear();
}
display::~display()
{

}
void display::render()
{
    // update a portion of the display with a render area
    uint8_t cmds[] = {
        SSD1306_SET_COL_ADDR,
        render_area.start_col,
        render_area.end_col,
        SSD1306_SET_PAGE_ADDR,
        render_area.start_page,
        render_area.end_page
    };

    SSD1306_send_cmd_list(cmds, count_of(cmds));
    SSD1306_send_buf();
}
void display::clear(bool immediate)
{
    memset(buf, 0, SSD1306_BUF_LEN);
    if (immediate)
    {
        render();
    }
}
void display::fill(bool immediate)
{
    memset(buf, 0xFF, SSD1306_BUF_LEN);
    if (immediate)
    {
        render();
    }
}
void display::WriteChar(int16_t x, int16_t y, uint8_t ch, bool immediate)
{
    if (x > SSD1306_WIDTH - 8 || y > SSD1306_HEIGHT - 8)
        return;

    // For the moment, only write on Y row boundaries (every 8 vertical pixels)
    y = y/8;
    if (ch == '.')
    {
        if (x < 8)
        {
            buf[y * 128 + x + 7] |= (1<<7);
        }
        else
        {
            buf[y * 128 + x - 1] |= (1<<7);
        }
    }
    else
    {
        ch = toupper(ch);
        int idx = GetFontIndex(ch);
        int fb_idx = y * 128 + x;

        for (int i=0;i<8;i++) {
            buf[fb_idx++] = font[idx * 8 + i];
        }
    }
    if (immediate)
    {
        render();
    }
}
void display::WriteString(int16_t x, int16_t y, char *str, bool immediate)
{
    // Cull out any string off the screen
    if (x > SSD1306_WIDTH - 8 || y > SSD1306_HEIGHT - 8)
        return;
    bool first = true;

    while (*str) {
        if (first)
        {
            first = false;
            WriteChar( x, y, *str++, false);
            x+=8;
        }
        else if (*str == '.')
        {
            WriteChar( x, y, *str++, false);
        }
        else
        {
            WriteChar( x, y, *str++, false);
            x+=8;
        }
    }
    if (immediate)
    {
        render();
    }
}
void display::_printf(uint line_nr, bool immediate, const char* format, ...)
{
    if (line_nr > 7)
    {
        printf("Display only has line 0 - 7\n");
        return;
    }
    memset(&buf[line_nr * 128], 0, 128);
    int16_t x = 0;
    int16_t y = line_nr * 8;
    va_list args;
    int count = 0;
    int printed = 0;
    int buff_ind = 0;
    uint total_chars = 17;
    char buffer[17];
    memset(buffer, 0, 17);
    va_start(args, format);
    while (*format)
    {
        if (*format == '%')
        {
            format ++;
            if (*format == '%')
            {
                buffer[buff_ind++] = '%';
                if (buff_ind == total_chars)
                {
                    goto end;
                }
            }
            else if (*format == 'c') // Case: '%c' prints a character
            {
                int ch = va_arg(args, int); // Fetch the next argument as int
                buffer[buff_ind++] = ch;
                if (buff_ind == total_chars)
                {
                    goto end;
                }
            }
            else if (*format == 's') // Case: '%s' prints a string
            {
                char *str = va_arg(args, char *); // Fetch the next argument as char*
                while (*str) // Iterate over each character in the string
                {
                    buffer[buff_ind++] = *str;
                    str++;
                    if (buff_ind == total_chars)
                    {
                        goto end;
                    }
                }
            }
            else if (*format == 'd' || *format == 'i') // Case: '%d' or '%i' prints an integer
            {
                int value = va_arg(args, int);
                // Convert the integer to a string representation and print it
                int printed = snprintf(&buffer[buff_ind], total_chars - buff_ind, "%d", value);
                buff_ind += printed;
                if (buff_ind == total_chars)
                {
                    goto end;
                }
            }
            else if (*format == 'f') // Case: '%d' or '%i' prints an integer
            {
                float value = va_arg(args, double);
                // Convert the integer to a string representation and print it
                int printed = snprintf(&buffer[buff_ind], total_chars - buff_ind, "%.2f", value);
                buff_ind += printed;
                if (buff_ind == total_chars)
                {
                    goto end;
                }
            }
        }
        else // Case: Regular character, not a conversion specifier
        {
            buffer[buff_ind++] = *format;
            if (buff_ind == total_chars)
            {
                goto end;
            }
        }
        format++;
    }
    end:
    WriteString(x, y, buffer, immediate);
    va_end(args);
}

void Motor_display::print_motor(bool dir, float speed)
    {
        clear(false);
        _printf(0, false, "Motor Control:");
        _printf(1, false, "Dir: %d", dir);
        _printf(2, true, "Speed: %f", speed);
    }