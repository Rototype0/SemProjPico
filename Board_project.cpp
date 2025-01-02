#include "pico/stdio.h"
#include "button.h"
#include "display.h"
#include "adc.h"
#include "motors.h"
#include "leds.h"
#include "tcp_server.h"

int main()
{
    stdio_init_all(); //initializing standard input output
    Button input_a(10); //creating button instance on gpio 10 with default settings
    Motor_display oled; //creating motor display
    Adc potentiometer(0, true); //creating potentiometer instance on channel 0(gpio 26) of the adc
    Servo_cont m1(21); //creating continuous servo motor on gpio 21
    PWM_LED led(7, 25, 100, 100, true); //create PWM controlled led on gpio 7 with 25 hertz frequency
    RGB_LED led_2(6, 0, 0xAE, 0x42, false, 100); //create RGB LED on gpio 6 
    LED_Base* leds[2]; //create led_base class pointer array
    leds[0] = &led; //feed reference of leds into led_base class pointer array
    leds[1] = &led_2;
    led.on(); //turn on pwm_led

    uint8_t cmd = 0; //initialize variable for commands from pc
    uint8_t prev_cmd = 0; //initialize variable for previous command from pc to only act on changes instead of everytime


    if (cyw43_arch_init()) { //initialize wifi
        printf("failed to initialise\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode(); //enable station mode

    oled._printf(0,true,"Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms("Pixel_5324", "88888888", CYW43_AUTH_WPA2_AES_PSK, 30000)) { //connect to wifi change SSID to your wifi SSID and pswd to your password
        printf("failed to connect.\n");
        return 1;
    } else {
        printf("Connected.\n");
    }

    oled.clear();
    TCP_SERVER_T *state = tcp_server_init(); //creating tcp server state
    if (!state) {
        goto end;
    }
    state->data.button_state = 0; //initializing all the data in the state to 0
    state->data.desk_height = 893;
    state->data.light_intesity = 0;
    state->data.pot_percent = 0;
    state->data.pressed = 0;
    state->data.pressed_since_last = 0;
    state->data.temp = 0;

    if (!tcp_server_open(state)) { //open server
        tcp_server_result(state, -1);
        goto end;
    }

    printf("%d\n", state->running);
    while(state->running != start) //wait for pc to connect and send start command
    {
        printf("%d\n", state->running);
        sleep_ms(500);
    }

 
    while(state->running != stop) //while pc hasn't sent stop command
    {

        uint32_t result = potentiometer.Read(); //read result from potentiometer
        if (result < deadzone) //check deadzone. Deadzone is defined in adc.h
        {
            result = 0;
        }
        float V = result * conversion_factor_V; //convert resut to different values. Conversion factors are defined in adc.h
        float degrees = result * conversion_factor_deg;
        float percent = result * conversion_factor_percent;

        /*
        space to put function for reading light sensor
        */

        bool dir = input_a.pressed_total()%2; //check if button has been pressed odd or even amount of times
        oled.print_motor(dir, percent); //print motor information
        int multiplier = 1; //define multiplier for going forward 
        for (unsigned int i = 0; i < 2; i++) //going through led base class pointer array to set the level of both leds
        {
            leds[i]->set_level(percent);
        }
        
        if (dir) //set direction of motor and reflect direction of motor in LEDS either being off or on
        {
            multiplier = -1;
            for (unsigned int i = 0; i < 2; i++)
            {
                leds[i]->on();
            }
        }
        else
        {
            for (unsigned int i = 0; i < 2; i++)
            {
                leds[i]->off();
            }
        }

        cmd = state->buffer_recv[0]; //read recieve buffer from tcp server state into cmd
        oled._printf(5, false, "Current Height: ");
        oled._printf(6, true, "%dcm", cmd);
        if (cmd != prev_cmd) //if cmd is not the same as previous
        {
            printf("command: %d\n", cmd); //do something cmd 0-3 are reserved for starting stopping or pausing
            prev_cmd = cmd; //set previous cmd to be cmd
            /*
            space for switch on the commands to do something fx turning motor on or off
            */
        }

        m1.Set_Speed(multiplier * percent); //set speed of motor
        state->data.button_state = input_a.is_pressed(); //set button state in tcp server state data to be equal to the value from button
        /*
        Space for setting rest of the data in state->data
        */
        tcp_server_send_data(state, state->client_pcb); //enqueue data to be sent
        while ((state->sent_len!=sizeof(data_to_send_t)) && (state->running==start)); //wait for message to have been sent
        sleep_ms(1000); //delay to not spam pc
    }
end:
    tcp_server_close(state); //close server
    free(state); //free memory of server state
    cyw43_arch_deinit(); // turn off wifi
    return 0;
}