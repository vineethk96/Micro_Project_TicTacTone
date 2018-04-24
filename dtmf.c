#include "dtmf.h"
#include "display.h"

// TO BE DEFINED BY YOU
//
// Compute Goertzel Filter Coefficients for
// Frequency Detectors for the following frequencies:
//
//      Gtap structure     Fgoertzel
//      -----------------------------
//      t697                 697 Hz
//      t770                 770 Hz
//      t852                 852 Hz
//      t1209               1209 Hz
//      t1336               1336 Hz
//      t1477               1477 Hz
//
// A Goertzel filter coefficient is defined by
//
//           c = 2*cos(2*pi*v)
//
// with v equal to Fgoertzel / Fsample.
// For example, if Fgoertzel = 1000Hz and Fsample = 8000Hz
// then c = 2 * cos (2*pi*1/8) = 1.4142
// Be sure to compute cos() in radians
//
// This results in the following filter coefficients
//
//      Gtap structure     Fgoertzel       Coefficient
//      ----------------------------------------------
//      t697                 697 Hz      0.8538689 * 2
//      t770                 770 Hz      0.8226405 * 2
//      t852                 852 Hz      0.7843435 * 2
//      t1209               1209 Hz      0.5820520 * 2
//      t1336               1336 Hz      0.4981851 * 2
//      t1477               1477 Hz      0.3993092 * 2
//
// To encode the Goertzel filter coefficient into Gtap,
// you have to convert the floating point value of c
// into a fixed point value.
//
// For a fixed point representation <WLEN,FRACBITS>,
// The resulting Gtap coef value is
//
//         coef =  (int) (c * (1 << FRACBITS))
//
// Make sure that you do NOT use double or float in your program.
// Use ONLY integer types (signed or unsigned).

// FILL IN THE FOLLOWING VALUES TO DEFINE PROPER COEF VALUES

static Gtap  t697 = {(int) (2 * 0.8538689 * (1 << FRACBITS)), 0, 0};
static Gtap  t770 = {(int) (2 * 0.8226405 * (1 << FRACBITS)), 0, 0};
static Gtap  t852 = {(int) (2 * 0.7843435 * (1 << FRACBITS)), 0, 0};
static Gtap t1209 = {(int) (2 * 0.5820520 * (1 << FRACBITS)), 0, 0};
static Gtap t1336 = {(int) (2 * 0.4981851 * (1 << FRACBITS)), 0, 0};
static Gtap t1477 = {(int) (2 * 0.3993092 * (1 << FRACBITS)), 0, 0};

void DTMFAddSample(unsigned x) {
    unsigned vx = ScaleSample(x);

    SampleGoertzel( &t697, vx);
    SampleGoertzel( &t770, vx);
    SampleGoertzel( &t852, vx);
    SampleGoertzel(&t1209, vx);
    SampleGoertzel(&t1336, vx);
    SampleGoertzel(&t1477, vx);
}

void DTMFReset() {
    ResetGoertzel( &t697);
    ResetGoertzel( &t770);
    ResetGoertzel( &t852);
    ResetGoertzel(&t1209);
    ResetGoertzel(&t1336);
    ResetGoertzel(&t1477);
}

void DTMFDisplay() {
    unsigned p697  = PowerGoertzel(  &t697);
    unsigned p770  = PowerGoertzel(  &t770);
    unsigned p852  = PowerGoertzel(  &t852);
    unsigned p1209 = PowerGoertzel( &t1209);
    unsigned p1336 = PowerGoertzel( &t1336);
    unsigned p1477 = PowerGoertzel( &t1477);

    DrawUnsigned(1, " 697",  p697);
    DrawUnsigned(2, " 770",  p770);
    DrawUnsigned(3, " 852",  p852);
    DrawUnsigned(4, "1209", p1209);
    DrawUnsigned(5, "1336", p1336);
    DrawUnsigned(6, "1477", p1477);
}

void DTMFPower()
{
    unsigned p697  = PowerGoertzel(  &t697);
    unsigned p770  = PowerGoertzel(  &t770);
    unsigned p852  = PowerGoertzel(  &t852);
    unsigned p1209 = PowerGoertzel( &t1209);
    unsigned p1336 = PowerGoertzel( &t1336);
    unsigned p1477 = PowerGoertzel( &t1477);

    unsigned sum = p697 + p770 + p852 + p1209 + p1336 + p1477;
    unsigned avg = sum / 6.0;

    DrawUnsigned(0, "Power", sum);
}

int DTMFlocation() {
    unsigned p697  = PowerGoertzel(  &t697);
    unsigned p770  = PowerGoertzel(  &t770);
    unsigned p852  = PowerGoertzel(  &t852);
    unsigned p1209 = PowerGoertzel( &t1209);
    unsigned p1336 = PowerGoertzel( &t1336);
    unsigned p1477 = PowerGoertzel( &t1477);

    if( (int)p697 > 10000 && (int)p1209 > 10000 )
        return 0;
    else if( (int)p697 > 10000 && (int)p1336 > 10000 )
        return 1;
    else if( (int)p697 > 10000 && (int)p1477 > 10000 )
        return 2;
    else if( (int)p770 > 10000 && (int)p1209 > 10000 )
        return 3;
    else if( (int)p770 > 10000 && (int)p1336 > 10000 )
        return 4;
    else if( (int)p770 > 10000 && (int)p1477 > 10000 )
        return 5;
    else if( (int)p852 > 10000 && (int)p1209 > 10000 )
        return 6;
    else if( (int)p852 > 10000 && (int)p1336 > 10000 )
        return 7;
    else if( (int)p852 > 10000 && (int)p1477 > 10000 )
        return 8;

    return 10;
}

