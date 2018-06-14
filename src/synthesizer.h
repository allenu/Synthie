//
// Synthie - Created by Allen Ussher, 2018
//
// If you use this, please credit me somewhere in your app or project.

#pragma once

#include "instrument.h"

typedef enum {
    kSynthesizerAction_PlayTone = 0,
    kSynthesizerAction_ReleaseTone,
} synthesizer_action_t;

typedef struct {
    instrument_t instrument;
    double freq;

    // TODO:
//    double phase;
//    double gain_factor;
    double note_play_time;
    bool note_released;
    double note_release_time;
} synthesizer_channel_t;

#define MAX_CHANNELS 16

typedef struct {
    int channel;
    double time;
    synthesizer_action_t action_type;

    // These are only used during PlayTone commands
    instrument_t instrument;
    double freq;
} synthesizer_command_t;

typedef struct {
    int num_channels;
    // TODO: avoid a fixed size here?
    synthesizer_channel_t channels[MAX_CHANNELS];
} synthesizer_state_t;

synthesizer_state_t f_next_synthesizer_state(const synthesizer_state_t & prev_state, 
        int num_commands, 
        const synthesizer_command_t *commands);

double f_synthesizer_sample(const synthesizer_state_t & synthesizer, double time);

