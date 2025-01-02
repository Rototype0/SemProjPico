#pragma once
#include "hardware/gpio.h"
#include "hardware/pwm.h"

/**
 * @brief basic motor class
 */
class Motor
{
protected:
    uint slice_num;
    uint gpio_m;
public:
    Motor(uint gpio_nr, uint16_t freq, uint16_t clk_div, uint16_t init_val=0, bool immidiate=true);
    ~Motor();
    void enable();
    void disable();
};


/**
 * @brief continuous servo motor class
 */
class Servo_cont : public Motor
{
private:
    float a_m;
    int b_m;
public:
    Servo_cont(uint gpio_nr, float init_speed=0, uint16_t freq=50, uint16_t clk_div=50, float a=12.5, int b=3749, bool immidiate=true);
    void Set_Speed(float speed);
};

/**
 * @brief angular servo motor class
 */
class Servo_angular : public Motor
{
private:
float range_m;
float a_m;
int b_m;
public:
    Servo_angular(uint gpio_nr, float angle=90, float range=180, uint16_t freq=50, uint16_t clk_div=50, float a=2500, int b=2499, bool immidiate=true);
    void Set_angle(float angle);
};

/**
 * @brief DC motor class
 */
class DC_Motor : public Motor
{
private:
    float mult;
public:
    DC_Motor(uint gpio_nr, float init_speed=0, uint16_t freq=2000, uint16_t clk_div=1, bool immidiate=true);
    void Set_speed(float speed);
};