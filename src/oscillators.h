//
// Synthie - Created by Allen Ussher, 2018
//
// If you use this, please credit me somewhere in your app or project.

#pragma once

typedef enum { 
    kSilence = 0,
    kSineWave,
    kTriangleWave,
    kSquareWave,
    kSawtoothWave,
    kNoiseWave,
} oscillator_t;

double f_oscillator_sample(oscillator_t oscillator, double freq, double start_time, double time);
