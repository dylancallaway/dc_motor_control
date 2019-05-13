#include "mbed.h"

DigitalOut grn_led(LED1);
DigitalOut blue_led(LED2);
DigitalOut red_led(LED3);

Thread blink_thread(osPriorityRealtime, 256);

void blink(void)
{
    while (true)
    {
        blue_led = !blue_led;
        wait(0.5);
    }
}

int main(void)
{
    blink_thread.start(blink);
}