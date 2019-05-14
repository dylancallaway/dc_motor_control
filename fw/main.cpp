#include "mbed.h"

Serial pc(USBTX, USBRX, 115200);

DigitalOut grn_led(LED1);
DigitalOut blue_led(LED2);
DigitalOut red_led(LED3);

Thread heartbeat_thread(osPriorityLow, 256);
void heartbeat_fcn();

#define ENC_1A_PIN D0
#define ENC_1B_PIN D1
InterruptIn enc_1a(ENC_1A_PIN);
InterruptIn enc_1b(ENC_1B_PIN);
void enc_1_isr();
volatile int64_t enc_1_count = 0;
static int8_t enc_lookup_table[16] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};

int main(void)
{
    heartbeat_thread.start(heartbeat_fcn);

    enc_1a.rise(&enc_1_isr);
    enc_1a.fall(&enc_1_isr);
    enc_1b.rise(&enc_1_isr);
    enc_1b.fall(&enc_1_isr);

    while (true)
    {
        pc.printf("%d\n", enc_1_count);
        wait(1);
    }
}

void enc_1_isr()
{
    static uint8_t enc_val = 0;
    enc_val = (enc_val << 2) | ((enc_1a.read() << 1) | enc_1b.read());
    enc_1_count += enc_lookup_table[enc_val & 0b1111];
}

void heartbeat_fcn()
{
    while (true)
    {
        grn_led = !grn_led;
        wait(0.5);
    }
}