#include "adc.h"

Adc::Adc(uint channel, bool first):channel_m(channel)
{
    if (channel_m < 4)
    {
        if (first)
        {
            adc_init();
        }
        uint gpio_nr;
        switch (channel_m)
        {
        case 0:
            gpio_nr = 26;
            break;
        case 1:
            gpio_nr = 27;
            break;
        case 2:
            gpio_nr = 28;
            break;
        case 3:
            gpio_nr = 0;
            adc_set_temp_sensor_enabled(true);
            break;
        default:
            break;
        }
        if (gpio_nr)
        {
            gpio_set_dir(gpio_nr, false);
            gpio_disable_pulls(gpio_nr);
            gpio_set_input_enabled(gpio_nr, false);
        }
    }
    else
    {
        printf("Not a valid channel");
    }
    
}

Adc::~Adc()
{
    if (channel_m == 3)
    {
        adc_set_temp_sensor_enabled(false);
    }
}

uint16_t Adc::Read()
{
    adc_select_input(channel_m);
    return adc_read();
}