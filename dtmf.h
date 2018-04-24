#ifndef DTMF_H_
#define DTMF_H_

#include "goertzel.h"

void     DTMFAddSample(unsigned x);
void     DTMFReset();
void     DTMFPower();
void     DTMFDisplay();
int      DTMFlocation();

#endif /* DTMF_H_ */
