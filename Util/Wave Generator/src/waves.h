#ifndef __WAVES_H
#define __WAVES_H

#define PI 3.14159265

typedef struct wave Wave_t;
typedef double (*Wave_Evaluate)(Wave_t *wave, double time);

struct wave
{
    double freqHz;
    double freqRad;
    double period;
    double phaseDeg;
    double phaseShift;
    double amplitude;
    double dcOffset;
    Wave_Evaluate evaluate;
};

void Waves_Sine_Setup(Wave_t *wave);

void Waves_Cosine_Setup(Wave_t *wave);

void Waves_Triangle_Setup(Wave_t *wave);

void Waves_Sawtooth_Setup(Wave_t *wave);

void Waves_Square_Setup(Wave_t *wave);

#endif /* __WAVES_H */