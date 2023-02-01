#include <math.h>

#include "waves.h"

static double sine_evaluate(Wave_t *wave, double time);
static double cosine_evaluate(Wave_t *wave, double time);
static double triangle_evaluate(Wave_t *wave, double time);
static double sawtooth_evaluate(Wave_t *wave, double time);
static double square_evaluate(Wave_t *wave, double time);

void trig_setup(Wave_t *wave)
{
    wave->period = (1.0 / wave->freqHz);
    wave->freqRad = 2.0 * PI * wave->freqHz;
    wave->phaseShift = 2.0 * PI * wave->phaseDeg;
}

void other_setup(Wave_t *wave)
{
    wave->period = (1.0 / wave->freqHz);
    wave->phaseShift = (wave->phaseDeg / 360.0) * wave->period;
}

void Waves_Sine_Setup(Wave_t *wave)
{
    trig_setup(wave);
    wave->evaluate = sine_evaluate;
}

void Waves_Cosine_Setup(Wave_t *wave)
{
    trig_setup(wave);
    wave->evaluate = cosine_evaluate;
}

void Waves_Triangle_Setup(Wave_t *wave)
{
    other_setup(wave);
    wave->evaluate = triangle_evaluate;
}

void Waves_Sawtooth_Setup(Wave_t *wave)
{
    other_setup(wave);
    wave->evaluate = sawtooth_evaluate;
}

void Waves_Square_Setup(Wave_t *wave)
{
    other_setup(wave);
    wave->evaluate = square_evaluate;
}

static double sine_evaluate(Wave_t *wave, double time)
{
    return (wave->amplitude * sin((wave->freqRad * time) + wave->phaseShift)) + wave->dcOffset;
}

static double cosine_evaluate(Wave_t *wave, double time)
{
    return (wave->amplitude * cos((wave->freqRad * time) + wave->phaseShift)) + wave->dcOffset;
}

static double triangle_evaluate(Wave_t *wave, double time)
{
    double Y;
    double timeBase = (time + wave->phaseShift) / wave->period;
    timeBase -= (long)timeBase;
    Y = 4.0 * timeBase;
    Y = (timeBase < 0.5) ? (Y - 1.0) : ((-1.0 * Y) + 3.0);

    return (wave->amplitude * Y) + wave->dcOffset;
}

static double sawtooth_evaluate(Wave_t *wave, double time)
{
    double timeBase = (time + wave->phaseShift) / wave->period;
    timeBase -= (long)timeBase;

    return ((2.0 * wave->amplitude * timeBase) - wave->amplitude) + wave->dcOffset;
}

static double square_evaluate(Wave_t *wave, double time)
{
    double timeBase = (time + wave->phaseShift) / wave->period;
    timeBase -= (long)timeBase;

    return (wave->amplitude * ((timeBase < 0.5) ? -1.0 : 1.0)) + wave->dcOffset;
}
