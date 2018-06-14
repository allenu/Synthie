//
// Synthie - Created by Allen Ussher, 2018
//
// If you use this, please credit me somewhere in your app or project.

#pragma once

#include "instrument.h"

typedef enum {
    kPatternAction_PlayTone = 0,
    kPatternAction_ReleaseTone,
} pattern_action_t;

typedef struct {
    int beat_index;
    pattern_action_t action_type;
    double freq;

    int channel;
    int instrument_index;
} pattern_command_t;

typedef struct {
    double bpm;
    int num_beats;  // How long the pattern is in beats

    int num_pattern_commands;
    pattern_command_t *pattern_commands;
} pattern_t;

typedef struct {
    int num_instruments;
    instrument_t *instruments;

    int num_patterns;
    pattern_t *patterns;

    int num_channels;
} song_t;

typedef struct {
    bool repeats;
    double delay_between_repetition;
    // double gain;
} song_reader_options_t;

typedef struct {
    song_reader_options_t song_reader_options;
    bool done;

    double pattern_start_time;  // when we started playing this pattern
    int pattern_index;          // which pattern we're playing in the song
    int beat_index;             // which note in the pattern we've processed

    // Cache of the index of the next command to execute in the current
    // pattern. This is just an optimization so that we don't search through 
    // the pattern_commands to find the next command with the same beat_index
    // as we need.
    int next_pattern_command_index;

    // Which commands to execute for this beat. Is just a pointer into 
    // pattern_commands for the given pattern in the song. These are all 
    // the commands for the current note.
    int num_pattern_commands;
    pattern_command_t *pattern_commands;
} song_reader_state_t;

song_reader_state_t f_next_song_reader_state(const song_reader_state_t & prev_state, const song_t & song, double time);

