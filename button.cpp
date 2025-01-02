#include "button.h"

Button::Button(uint gpio_nr, uint32_t debounce_time, bool trigger_on_press, bool enable, bool pull_up)
{
    gpio = gpio_nr;
    type = trigger_on_press;
    pressed_since_last = 0;
    gpios_and_types[gpio].gpio = gpio;
    gpios_and_types[gpio].type = type;
    debounce_times[gpio] = debounce_time;
    pressed_counter[gpio] = 0;
    cleared_h[gpio] = true;
    cleared_l[gpio] = true;
    gpio_set_dir(gpio, false);
    if (pull_up)
    {
        gpio_pull_up(gpio);
    }
    else
    {
        gpio_pull_down(gpio);
    }
    if (button_count == 0)
    {
        gpio_set_irq_enabled_with_callback(gpio, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &Button_press_cb);
    }
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, enable);
    button_count ++;    
}
Button::~Button()
{
    disable();
    button_count --;
}
void Button::enable()
{
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
}
void Button::disable()
{
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, false);
}
bool Button::is_pressed()
{
    if (type)
    {
        return (1<<gpio) & button_mask;
    }
    return !((1<<gpio) & button_mask);
}
uint Button::pressed_total()
{
    return pressed_counter[gpio];
}
uint Button::pressed_since_last_check()
{
    uint res = pressed_counter[gpio] - pressed_since_last;
    pressed_since_last = pressed_counter[gpio];
    return res;
}
