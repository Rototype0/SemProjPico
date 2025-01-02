#pragma once
#include "hardware/adc.h"
#include "stdio.h"

#define deadzone 0x016
#define conversion_factor_V 3.3f / (1 << 12)
#define conversion_factor_deg 270.0f / (1 << 12)
#define conversion_factor_percent 100.0f / (1 << 12)
/**
 * @brief Class for using the adc
 */
class Adc
{
protected:
    uint channel_m; //channel number of this instance
public:
/**
 * @brief constructor
 * @param channel channel to use for adc
 * @param first whether it is first adc instance
 */
    Adc(uint channel, bool first);
    ~Adc();
    /**
     * @return returns current value on this instance's adc channel
     */
    uint16_t Read();
};