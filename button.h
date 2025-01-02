#pragma once
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

struct gpio_and_type_t
{
    uint gpio;
    bool type;
};


static uint32_t button_mask; //mask containing up to 32 values for buttons
static uint button_count = 0; //amount of buttons created
static gpio_and_type_t gpios_and_types[32]; //array containing gpios and types for each possible button
static uint pressed_counter[32]; //array for total pressed for each possible button
static uint32_t debounce_times[32]; //array for debounce times for each possible button
static bool cleared_h[32]; //array for clear rising edge flag for each possible button
static bool cleared_l[32]; //array for clear falling edge flag for each possible button


/**
 * @brief Class for handling button stuff
 */
class Button
{
private:
    uint gpio; //gpio associated with current instance
    bool type; //count on rising or falling edge
    uint pressed_since_last; //variable for keeping track of the button presses previous time checked since last function was called
    static int64_t Timer_cb_h(alarm_id_t id, void *userdata)
    {
        if (gpio_get(((gpio_and_type_t*)userdata)->gpio))
        {
            if (((gpio_and_type_t*)userdata)->type)
            {
                pressed_counter[((gpio_and_type_t*)userdata)->gpio] ++;
            }
            button_mask |= (1<<((gpio_and_type_t*)userdata)->gpio);
        }
        cleared_h[((gpio_and_type_t*)userdata)->gpio] = true;
        return 0;
    }
    static int64_t Timer_cb_l(alarm_id_t id, void *userdata)
    {
        if (!gpio_get(((gpio_and_type_t*)userdata)->gpio))
        {
            if (!((gpio_and_type_t*)userdata)->type)
            {
                pressed_counter[((gpio_and_type_t*)userdata)->gpio] ++;
            }
            button_mask &= !(1<<((gpio_and_type_t*)userdata)->gpio);
        }
        cleared_l[((gpio_and_type_t*)userdata)->gpio] = true;
        return 0;
    }
    static void Button_press_cb(uint gpio_nr, uint32_t events)
    {
        if ((events == GPIO_IRQ_EDGE_RISE) && cleared_h[gpio_nr])
        {
            cleared_h[gpio_nr] = false;
            add_alarm_in_ms(debounce_times[gpio_nr], Timer_cb_h, &(gpios_and_types[gpio_nr]), false);
        }
        if ((events == GPIO_IRQ_EDGE_FALL) && cleared_l[gpio_nr])
        {
            cleared_l[gpio_nr] = false;
            add_alarm_in_ms(debounce_times[gpio_nr], Timer_cb_l, &(gpios_and_types[gpio_nr]), false);
        }
    }
public:
    /**
     * @brief Initializing Button class
     * @param gpio_nr gpio nr to initialize the button on
     * @param debounce_time time in milisecond input needs to be stable before reacting (default is 50)
     * @param trigger_on_press triggers on press or release
     * @param enable enable button
     * @param pull_up does button need pull up
     */
    Button(uint gpio_nr, uint32_t debounce_time=50, bool trigger_on_press=true, bool enable=true, bool pull_up=false);
    /**
     * @brief Destructor
     */
    ~Button();
    /**
     * @brief Enables button
     */
    void enable();
    /**
     * @brief Disables button
     */
    void disable();
    /**
     * @brief Checks current state of button
     * @return Returns true if button is pressed
     */
    bool is_pressed();
    /**
     * @return Returns the total amount of times the button has been pressed
     */
    uint pressed_total();
    /**
     * @return Returns the amount of times the button has been pressed since last calling this function
     */
    uint pressed_since_last_check();
};