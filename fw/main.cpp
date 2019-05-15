#include "mbed.h"

// Helpers
void clamp(float *val, float min, float max);

// Serial
Serial pc(USBTX, USBRX, 115200);

// LED
DigitalOut grn_led(LED1);
DigitalOut blue_led(LED2);
DigitalOut red_led(LED3);
Thread heartbeat_thread(osPriorityLow, 256);
void heartbeat_fcn();

// Encoders
static int8_t enc_lookup_table[16] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};
// Encoder 1
#define ENC_1A_PIN D0
#define ENC_1B_PIN D1
InterruptIn enc_1a(ENC_1A_PIN);
InterruptIn enc_1b(ENC_1B_PIN);
void enc_1_isr();
volatile int64_t enc_1_count = 0;

// Motors
// Motor 1
#define MOT_1A_PIN PC_8
#define MOT_1B_PIN PC_9
PwmOut mot_1a(MOT_1A_PIN);
PwmOut mot_1b(MOT_1B_PIN);

// Controllers
// Controller 1
Ticker controller_1_ticker;
void controller_1_fcn();

int main()
{
    // Encoder 1 init
    enc_1a.rise(&enc_1_isr);
    enc_1a.fall(&enc_1_isr);
    enc_1b.rise(&enc_1_isr);
    enc_1b.fall(&enc_1_isr);

    // Motor 1 init
    mot_1a.period_ms(1);
    mot_1b.period_ms(1);
    mot_1a.write(0.0);
    mot_1b.write(0.0);

    // Controller 1
    controller_1_ticker.attach_us(&controller_1_fcn, 100);

    // Heartbeat init after other inits
    heartbeat_thread.start(heartbeat_fcn);

    // Infinite loop
    while (true)
    {
        pc.printf("%d\n", enc_1_count);
        wait(0.5);
    }
}

void controller_1_fcn()
{
    static int64_t err = 0;
    static int64_t set = 10000;
    static float Kp = 0.01;
    static float out = 0;

    if (out > 0)
    {
        mot_1a.write(out);
        mot_1b.write(0.0);
    }
    else if (out < 0)
    {
        mot_1a.write(0.0);
        mot_1b.write(-out);
    }

    err = set - enc_1_count;
    out = Kp * err;
    clamp(&out, -1.0, 1.0);
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

void clamp(float *val, float min, float max)
{
    if (*val < min)
    {
        *val = min;
    }
    else if (*val > max)
    {
        *val = max;
    }
}