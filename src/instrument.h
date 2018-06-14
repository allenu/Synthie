//
// Synthie - Created by Allen Ussher, 2018
//
// If you use this, please credit me somewhere in your app or project.

#pragma once

#include "oscillators.h"
#include "envelope.h"

typedef struct {
    oscillator_t oscillator;
    envelope_t envelope;
} instrument_t;

double f_instrument_sample(const instrument_t & instrument, double freq, double note_play_time, bool note_released, double note_release_time, double time);
