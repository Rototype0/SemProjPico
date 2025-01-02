#include "leds.h"

LED_Base::LED_Base(uint gpio_nr):gpio_m(gpio_nr)
{
}

LED_Base::~LED_Base()
{
    
}

LED::LED(uint gpio_nr):LED_Base(gpio_nr)
{
    gpio_init(gpio_m);
    gpio_set_dir(gpio_m, true);
}

LED::~LED()
{
    off();
}

void LED::on()
{
    gpio_put(gpio_m, true);
}

void LED::off()
{
    gpio_put(gpio_m, false);
}

PWM_LED::PWM_LED(uint gpio_nr, uint16_t freq, uint16_t clk_div, float init_val, bool immidiate):LED_Base(gpio_nr)
{
    gpio_set_function(gpio_m, GPIO_FUNC_PWM);
    // Find out which PWM slice is connected to the chosen GPIO pin
    slice_num = pwm_gpio_to_slice_num(gpio_m);
    uint16_t top = 125000000 / (clk_div * freq) - 1; // Calculating the 'top' value for 50Hz frequency
    mult = top/100;
    brightness = init_val;
    if(brightness > 100)
    {
        brightness = 100;
    }
    else if (brightness < 0)
    {
        brightness = 0;
    }
    
    pwm_set_clkdiv(slice_num, clk_div);
    pwm_set_wrap(slice_num, top);
    pwm_set_gpio_level(gpio_m, mult * brightness);
    pwm_set_enabled(slice_num, immidiate);
}

PWM_LED::~PWM_LED()
{
    off();
    pwm_set_enabled(slice_num, false);
}

void PWM_LED::set_level(float level)
{
    brightness = level;
    if(brightness > 100)
    {
        brightness = 100;
    }
    else if (brightness < 0)
    {
        brightness = 0;
    }
    pwm_set_gpio_level(gpio_m, mult * brightness);
}

void PWM_LED::on()
{
    pwm_set_gpio_level(gpio_m, mult * brightness);
}

void PWM_LED::off()
{
    pwm_set_gpio_level(gpio_m, 0);
}

RGB_LED::RGB_LED(uint gpio_nr, uint8_t r, uint8_t g, uint8_t b, bool on, float level):LED_Base(gpio_nr), r_m(r), g_m(g), b_m(b), level_m(level), on_m(on),
value(((uint32_t) (r) << 8) |
        ((uint32_t) (g) << 16) |
        (uint32_t) (b))
{
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, 6, 800000, false);
    pio_sm_put_blocking(pio0, 0, on ? (value << 8u) : 0);
    sleep_us(50);
}

RGB_LED::~RGB_LED()
{
    off();
}

void RGB_LED::on()
{
    on_m = true;
    pio_sm_put_blocking(pio0, 0, value << 8u);
    sleep_us(50);
}

void RGB_LED::off()
{
    on_m = false;
    pio_sm_put_blocking(pio0, 0, (uint32_t)0);
    sleep_us(50);
}

void RGB_LED::set_level(float level)
{
    level_m = level;
    value = (((uint32_t) ((float)r_m * level_m / 100.0f)) << 8) |
        (((uint32_t) ((float)g_m * level_m / 100.0f)) << 16) |
        ((uint32_t) ((float)b_m * level_m / 100.0f));
    if (on_m)
        pio_sm_put_blocking(pio0, 0, value << 8u);
    sleep_us(50);
}

void RGB_LED::set_colors(uint8_t r, uint8_t g, uint8_t b, bool on)
{
    r_m = r;
    g_m = g;
    b_m = b;
    value = ((uint32_t) ((float)r_m * level_m / 100.0f) << 8) |
        ((uint32_t) ((float)g_m * level_m / 100.0f) << 16) |
        (uint32_t) ((float)b_m * level_m / 100.0f);
    pio_sm_put_blocking(pio0, 0, on ? value << 8u : 0);
    sleep_us(50);
}