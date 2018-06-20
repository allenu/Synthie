#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include "oscillators.h"

// Normalize an angle theta from 0.0 to 2PI
double normalize_angle(double theta) {
    if (theta < 0) {
        int multiples = theta / (2.0*M_PI);
        theta += (2.0*M_PI) * (multiples + 1);
    }
    theta = fmod(theta, 2.0*M_PI);
    return theta;
}

double sawtooth(double theta) {
    theta = normalize_angle(theta);

    if (theta < M_PI ) {
        return theta / M_PI;
    } else {
        return -1.0 + (theta - M_PI) / M_PI;
    }
}

//
// Triangle wave
//
double triwave(double theta) {
    theta = normalize_angle(theta);
    
    if (theta < M_PI / 2.0) {
        return theta / (M_PI / 2.0);
    } else if (theta < 3.0 * M_PI / 2.0) {
        return 1.0 - 2.0 * (theta - (M_PI / 2.0)) / M_PI;
    } else {
        return (theta - 3.0 * M_PI / 2.0) / (M_PI / 2.0);
    }
}

double smooth_triwave(double theta) {
    theta = normalize_angle(theta);

    return (triwave(theta) + sin(theta)) / 2.0;
}

// Duty cycle = 0.0 to 1.0
double squarewave(double theta, double duty_cycle) {
    theta = normalize_angle(theta);
    
    if (theta < 2.0*M_PI*duty_cycle)
        return 1.0;
    else
        return -1.0;   
}

double noisewave(double theta) {
    theta = normalize_angle(theta);

    double sample = squarewave(theta, 0.5) * sin(theta);
    double randomGain = (rand() % 1000) / 1000.0;
    
    sample *= randomGain;
    
    return sample;
}

// Oscillates from -1 to +1
double f_oscillator_sample(oscillator_t oscillator, double freq, double start_time, double time) {
    double normalized_time = time - start_time;
    double theta = 2.0 * M_PI * freq * normalized_time;

    switch (oscillator) {
        case kSineWave:
            return sin(theta);

        case kTriangleWave:
            return triwave(theta);

        case kSquareWave:
            return squarewave(theta, 0.70);

        case kSawtoothWave:
            return sawtooth(theta);

        case kNoiseWave:
            return noisewave(theta);

        case kSilence:
            return 0.0;

        default:
            assert(false);
    }
}

