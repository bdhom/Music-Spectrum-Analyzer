// Used to generate a discrete sine wave from the command line

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "waves.h"

#define DEGREE_SYMBOL 248
#define MAX_WAVES     100

int get_option(int argc, char **argv, int *const index);
void expected_usage(void);
void print_wave(char *name, Wave_t *wave);
void parse_wave_args(char **argv, double *freq, double *amplitude, double *phase, double *dcOffset);
void generate_wave(double sampleFreq, double period);
double evaluate_all(double time);

int wave_count = 0;
Wave_t waves[MAX_WAVES];

int main(int argc, char **argv)
{
    double sampleFreq, period;
    int opt;
    char *temp;
    Wave_t *curWave;

    int optIndex = 1;

    printf("\n");

    if (argc < 3)
    {
        printf("Too few arguments - no wave to generate\n");
        expected_usage();

        return 1;
    }

    sampleFreq = strtod(argv[1], &temp);
    period = strtod(argv[2], &temp);

    while ((opt = get_option(argc, argv, &optIndex)) != -1)
    {
        if (optIndex + 3 >= argc)
        {
            printf("Malformed option arrangement for option: %c\n", opt);
            expected_usage();

            return 1;
        }

        curWave = &waves[wave_count];
        parse_wave_args(&argv[optIndex], &curWave->freqHz, &curWave->amplitude, &curWave->phaseDeg,
                        &curWave->dcOffset);

        if (opt == 's')
        {
            print_wave("Sine Wave", curWave);
            Waves_Sine_Setup(curWave);
        }
        else if (opt == 'c')
        {
            print_wave("Cosine Wave", curWave);
            Waves_Cosine_Setup(curWave);
        }
        else if (opt == 't')
        {
            print_wave("Triangle Wave", curWave);
            Waves_Triangle_Setup(curWave);
        }
        else if (opt == 'w')
        {
            print_wave("Sawtooth Wave", curWave);
            Waves_Sawtooth_Setup(curWave);
        }
        else if (opt == 'q')
        {
            print_wave("Square Wave", curWave);
            Waves_Square_Setup(curWave);
        }
        else
        {
            printf("Unknown option: %c\n", opt);
            expected_usage();

            return 1;
        }

        wave_count++;
    }

    generate_wave(sampleFreq, period);

    return 0;
}

int get_option(int argc, char **argv, int *const index)
{
    while (*index < argc)
    {
        if (argv[*index][0] == '-')
        {
            ++(*index);
            return argv[*index - 1][1];
        }

        ++(*index);
    }

    return -1;
}

void expected_usage(void)
{
    printf("\nWave Generator\n\n");

    printf("Description:\n");
    printf("A discrete wave generator, users can combine several different wave types to produce\n"
           "a discrete wave representation. Items such as frequency, amplitude, phase, and DC\n"
           "offset can all be adjusted.\n\n");

    printf("Usage:\n");
    printf("  wave_generator <sample_freq> <period> "
           "(-<wave> <freq> <amplitude> <phase> <dc>)...\n\n");

    printf("Wave Options:\n");
    printf("  -s\tProduces a sine wave.\n");
    printf("  -c\tProduces a cosine wave.\n");
    printf("  -t\tProduces a triangle wave.\n");
    printf("  -w\tProduces a sawtooth wave.\n");
    printf("  -q\tProduces a square wave.\n\n");
}

void print_wave(char *name, Wave_t *wave)
{
    printf("%s - Frequency: %.3f Hz, Amplitude: %.3f, Phase: %.3f%c, DC Offset: %.3f\n", name,
           wave->freqHz, wave->amplitude, wave->phaseDeg, DEGREE_SYMBOL, wave->dcOffset);
}

void parse_wave_args(char **argv, double *freq, double *amplitude, double *phase, double *dcOffset)
{
    char *temp;

    *freq = strtod(argv[0], &temp);
    *amplitude = strtod(argv[1], &temp);
    *phase = strtod(argv[2], &temp);
    *dcOffset = strtod(argv[3], &temp);
}

void generate_wave(double sampleFreq, double period)
{
    double cur = 0.0;
    double inc = 1.0 / sampleFreq;
    int i = 0;

    printf("Generated Wave:\n");
    printf("{\n");

    do
    {
        if (i % 4 == 0 && i != 0)
        {
            printf("\n");
        }

        printf(" %10f,", evaluate_all(cur));
        i++;
    } while ((cur += inc) < period);

    printf("\n}\n");
    printf("Number of Elements: %d\n\n", i);
}

double evaluate_all(double time)
{
    double retVal = 0.0;

    for (int i = 0; i < wave_count; i++)
    {
        retVal += waves[i].evaluate(&waves[i], time);
    }

    return retVal;
}
