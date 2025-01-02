#include "motors.h"

 Motor::Motor(uint gpio_nr, uint16_t freq, uint16_t clk_div, uint16_t init_val, bool immidiate)
{
    // Initialize the chosen GPIO pin
    gpio_m = gpio_nr;
    gpio_set_function(gpio_m, GPIO_FUNC_PWM);

    // Find out which PWM slice is connected to the chosen GPIO pin
    slice_num = pwm_gpio_to_slice_num(gpio_m);
    uint16_t top = 125000000 / (clk_div * freq) - 1; // Calculating the 'top' value for 50Hz frequency
    pwm_set_clkdiv(slice_num, clk_div);
    pwm_set_wrap(slice_num, top);
    pwm_set_gpio_level(gpio_m, init_val);
    pwm_set_enabled(slice_num, immidiate);
}
Motor::~Motor()
{
    disable();
}
void Motor::enable()
{
    pwm_set_enabled(slice_num, true);
}
void Motor::disable()
{
    pwm_set_enabled(slice_num, false);
}


Servo_cont::Servo_cont(uint gpio_nr, float init_speed, uint16_t freq, uint16_t clk_div, float a, int b, bool immidiate):Motor(gpio_nr, freq, clk_div, (init_speed*a) + b, immidiate), a_m(a), b_m(b)
{}

void Servo_cont::Set_Speed(float speed)
{
    if (speed > 100)
    {
        speed = 100;
    }
    else if (speed < -100)
    {
        speed = -100;
    }
    pwm_set_gpio_level(gpio_m, speed * a_m + b_m);
}

Servo_angular::Servo_angular(uint gpio_nr, float angle, float range, uint16_t freq, uint16_t clk_div, float a, int b, bool immidiate):Motor(gpio_nr, freq, clk_div, ((angle/range)*a+b), immidiate), range_m(range), a_m(a), b_m(b)
{}

void Servo_angular::Set_angle(float angle)
{
    if (angle < 0)
    {
        angle = 0;
    }
    else if (angle > range_m)
    {
        angle = range_m;
    }
    pwm_set_gpio_level(gpio_m, ((angle/range_m)*a_m+b_m));
}

DC_Motor::DC_Motor(uint gpio_nr, float init_speed, uint16_t freq, uint16_t clk_div, bool immidiate):Motor(gpio_nr, freq, clk_div, (((float)init_speed)*((125000000 / (clk_div * freq) - 1)/100)), immidiate), mult(((125000000 / (clk_div * freq) - 1)/100))
{}

void DC_Motor::Set_speed(float speed)
{
    if (speed > 100)
    {
        speed = 100;
    }
    else if (speed < -100)
    {
        speed = -100;
    }
    pwm_set_gpio_level(gpio_m, speed * mult);
}