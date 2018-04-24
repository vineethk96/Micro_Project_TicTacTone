#include "sound.h"
#include "hwtimer.h"
#include "swtimer.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

// This configuration sets a 50% duty cycle configuration
// for CCR4. You will have to figure out what Timer_A module
// drives the buzzer, i.e. what pin TAx.4 is driving the
// buzzer. The Timer_A module then is x.

#define CARRIER ((int) (SYSTEMCLOCK / 64000))


Timer_A_PWMConfig pwmConfig = {
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_1,
        (int) (SYSTEMCLOCK / 523.25),         // C5
        TIMER_A_CAPTURECOMPARE_REGISTER_4,
        TIMER_A_OUTPUTMODE_RESET_SET,
        (int) ((SYSTEMCLOCK/2) / 523.25)
};


void InitSound() {

    // This function switches the IC pin connected to
    // the buzzer from GPIO functionality to Timer_A functionality
    // so that we can drive it with PWM.

    GPIO_setAsPeripheralModuleFunctionOutputPin(
            GPIO_PORT_P2,
            GPIO_PIN7,
            GPIO_PRIMARY_MODULE_FUNCTION);
}

void PlaySound(tnote n, unsigned ms) {

    //=============================================================
    // TO BE COMPLETED BY YOU

    // Play note n for ms milliseconds.

    // You have to use the PWM setting of the Timer_A
    // peripheral that drives the buzzer to sound it

    // The delay ms is generated using a software timer
    // (different from Timer_A!)

    // PlaySound can be implemented as a blocking function.
    // That means that the function turns on the PWM
    // generation, then waits for ms milliseconds, then
    // turns the PWM generation off again.

    uint32_t freq;
    bool isSilent = false;

    switch(n)
    {
    case note_silent:
        pwmConfig.timerPeriod = 0;
        pwmConfig.dutyCycle = 0;
        isSilent = true;
        break;
    case note_c4:
        freq = 261.63;
        break;
    case note_d4:
        freq = 293.66;
        break;
    case note_e4:
        freq = 329.63;
        break;
    case note_f4:
        freq = 349.23;
        break;
    case note_g4:
        freq = 392.00;
        break;
    case note_a4:
        freq = 440.00;
        break;
    case note_b4:
        freq = 493.88;
        break;
    case note_c5:
        freq = 523.25;
        break;
    case note_d5:
        freq = 587.33;
        break;
    case note_e5:
        freq = 659.25;
        break;
    case note_f5:
        freq = 698.46;
        break;
    case note_fs5:
        freq = 739.99;
        break;
    case note_g5:
        freq = 783.99;
        break;
    case note_a5:
        freq = 880.00;
        break;
    case note_b5:
        freq = 987.77;
        break;
    case note_c6:
        freq = 1046.50;
        break;
    }

    if(!isSilent)
    {
        pwmConfig.timerPeriod = (int) (SYSTEMCLOCK / freq);
        pwmConfig.dutyCycle =  (int) ((SYSTEMCLOCK/2) / freq);
    }
    else
    {
        isSilent = false;
    }

    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);

    uint32_t timeConv = 3000 * ms;

    tSWTimer oneshotMS;
    InitSWTimer (&oneshotMS, TIMER32_1_BASE, timeConv);
    StartSWTimer(&oneshotMS);

    while(1)
    {
        if(SWTimerOneShotExpired(&oneshotMS))
        {
            break;
        }
    }
    return;


    //=============================================================

}
