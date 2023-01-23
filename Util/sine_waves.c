// Used to generate a discrete sine wave from the command line

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265

void expected_use(void);
void generate_sine(int cycles, int size);

int main(int argc, char **argv)
{
    int numElements, cycles;

    if (argc != 3)
    {
        expected_use();

        return -1;
    }

    numElements = atoi(argv[1]);
    cycles = atoi(argv[2]);

    generate_sine(cycles, numElements);

    return 0;
}

void expected_use(void)
{
    printf("Expected use:\n");
    printf("[exe] [num_elements] [cycles]\n");
}

/// @brief Given a set amount of complete cycles and the size of the array
/// create a discrete  sine wave
/// @param cycles Complete cycles of sine wave
/// @param size Size of the array that will determine angle step
void generate_sine(int cycles, int size)
{
    int i;
    double inc = ((2.0 * PI) / (double)size) * cycles;

    printf("{");

    for (i = 0; i < (size - 1); i++)
    {
        if (i % 4 == 0 && i != 0)
        {
            printf("\n");
        }

        printf(" %f,", sin(inc * i));
    }

    if (i % 4 == 0)
    {
        printf("\n");
    }

    printf(" %f }\n", sin(inc * i));
}
