#ifndef GOERTZEL_H_
#define GOERTZEL_H_

#define PTHRESHOLD 1000
#define FRACBITS 8
#define FIXMUL(A, B) ((A * B) >> FRACBITS)

typedef struct {
    int coef;
    int s2, s1;
} Gtap;

int  ScaleSample   (unsigned s);
void SampleGoertzel(Gtap *t, unsigned x);
void ResetGoertzel (Gtap *t);
int  PowerGoertzel (Gtap *t);

#endif /* GOERTZEL_H_ */
