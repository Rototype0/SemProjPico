#pragma once
#include "hardware/pwm.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"
#include "pico/stdlib.h"
/**
 * @brief LED base class interface class, do not use!!
 */
class LED_Base
{
protected:
    uint gpio_m;
public:
    LED_Base(uint gpio_nr);
    ~LED_Base();
    virtual void on(){}
    virtual void off(){}
    /**
     * @brief interface for setting level
     */
    virtual void set_level(float level){}
};


/**
 * @brief basic led with on/off function
 */
class LED : public LED_Base
{
public:
    LED(uint gpio_nr);
    ~LED();
    void on();
    void off();
};

/**
 * @brief led with brightness controlled by pwm
 */
class PWM_LED : public LED_Base
{
private:
    float brightness;
    uint slice_num;
    float mult;
public:
/**
 * @brief constructor for PWM_LED
 * @param gpio_nr number of the gpio pin for the led
 * @param freq desired pwm frequency
 * @param clk_div desired clock divider
 * @param init_val initial brightness off the led
 * @param immidiate turn on pwm immediately
 */
    PWM_LED(uint gpio_nr, uint16_t freq, uint16_t clk_div, float init_val=0, bool immidiate=true);
    ~PWM_LED();
    /**
     * @brief sets brightness of LED
     * @param level desired brightness level between 0 and 100
     */
    void set_level(float level);
    void on();
    void off();
};


/**
 * @brief class for RGB LED
 */
class RGB_LED : public LED_Base
{
private:
    bool on_m;
    uint8_t r_m, g_m, b_m;
    uint32_t value;
    float level_m;
public:
/**
 * @brief constructor for RGB LED
 * @param gpio_nr gpio number for led
 * @param r red value 0-255
 * @param g green value 0-255
 * @param b blue value 0-255
 * @param on initial value for on or off
 * @param level initial brightness level level
 */
    RGB_LED(uint gpio_nr, uint8_t r, uint8_t g, uint8_t b, bool on, float level);
    ~RGB_LED();
    void on();
    void off();
    /**
     * @brief sets brightness of LED
     * @param level desired brightness level between 0 and 100
     */
    void set_level(float level);
    /**
     * @brief sets colours
     * @param r red value 0-255
     * @param g green value 0-255
     * @param b blue value 0-255
     * @param on value for on or off
     */
    void set_colors(uint8_t r, uint8_t g, uint8_t b, bool on);
};