#pragma once

#include "songreader.h"
#include "synthesizer.h"

// A song player uses a song reader to read a song and get pattern commands from it.
// These pattern commands are then fed to a synthesizer, which provides the audio
// samples.
typedef struct {
    song_reader_state_t song_reader_state;
    synthesizer_state_t synthesizer_state;
    double next_sample_time;

    double next_beat_time;
} song_player_state_t;

// API

song_player_state_t create_song_player_state(int num_channels);

// Given a song player and a song, get the audio samples for a given sample rate.
// This also produces the next state for the player to feed in for the next time.
void get_song_player_samples(
        const song_t & song,
        const song_player_state_t & prev_state, 
        const double sample_rate,
        const int num_samples,
        double *samples,
        song_player_state_t *next_state);

