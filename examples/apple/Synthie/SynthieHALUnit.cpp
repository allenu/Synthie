//
//  SynthieHALUnit.cpp
//  Synthie
//
//  Created by Allen Ussher on 6/10/18.
//  Copyright © 2018 Ussher Press. All rights reserved.
//

#include "SynthieHALUnit.hpp"
#include "song_from_file.h"

static double s_notes[12] = {16.35, 17.32, 18.35, 19.45,
    20.60,21.83,23.12,24.50,
    25.96,27.50,29.14,30.87};

#define Note_C   s_notes[0]*16
#define Note_D   s_notes[2]*16
#define Note_E   s_notes[4]*16
#define Note_F   s_notes[5]*16
#define Note_G   s_notes[7]*16
#define Note_A   s_notes[9]*16
#define Note_B   s_notes[10]*16
#define Note_C2  s_notes[11]*16

static instrument_t s_instruments[] = {
    { kSquareWave,   { 0.01, 0.5, 0.01, 0.3, 0.5 } },
    { kTriangleWave,     { 0.025, 1.0, 0.025, 0.8, 0.5 } },
    { kNoiseWave,    { 0.025, 0.7, 0.025, 0.5, 0.05 } },
};

#define Bass_Instrument 0
#define Horn_Instrument 1
#define Snare_Instrument 2

static pattern_command_t s_pattern0_commands[] = {
    pattern_command_t { 0, kPatternAction_PlayTone, Note_C, 0, Bass_Instrument },
    pattern_command_t { 0, kPatternAction_PlayTone, Note_C, 1, Horn_Instrument },
    pattern_command_t { 1, kPatternAction_PlayTone, Note_E, 2, Horn_Instrument },
    pattern_command_t { 2, kPatternAction_PlayTone, Note_C, 0, Bass_Instrument },
    pattern_command_t { 2, kPatternAction_PlayTone, Note_C, 3, Snare_Instrument },
    pattern_command_t { 2, kPatternAction_ReleaseTone, 0.0, 1, 0 },
    pattern_command_t { 3, kPatternAction_ReleaseTone, 0.0, 2, 0 },
    pattern_command_t { 3, kPatternAction_ReleaseTone, 0.0, 3, 0 },
    pattern_command_t { 4, kPatternAction_PlayTone, Note_F, 0, Bass_Instrument },
    pattern_command_t { 4, kPatternAction_PlayTone, Note_F, 1, Horn_Instrument },
    pattern_command_t { 5, kPatternAction_PlayTone, Note_A, 2, Horn_Instrument },
    pattern_command_t { 6, kPatternAction_PlayTone, Note_F, 0, Bass_Instrument },
    pattern_command_t { 6, kPatternAction_PlayTone, Note_C, 3, Snare_Instrument },
    pattern_command_t { 7, kPatternAction_ReleaseTone, 0.0, 1, 0 },
    pattern_command_t { 7, kPatternAction_ReleaseTone, 0.0, 2, 0 },
    pattern_command_t { 7, kPatternAction_ReleaseTone, 0.0, 3, 0 },
};
static pattern_t s_patterns[] = {
    { 120.0, 1, 8, sizeof(s_pattern0_commands)/sizeof(s_pattern0_commands[0]), s_pattern0_commands },
};

SynthieHALUnit::SynthieHALUnit(const char *filename) {
#if 0
    // Load song via s_patterns
    song_t song = { 0 };
    
    song.num_instruments = sizeof(s_instruments)/sizeof(s_instruments[0]);
    song.instruments = s_instruments;
    
    song.num_patterns = sizeof(s_patterns)/sizeof(s_patterns[0]);
    song.patterns = s_patterns;
    song.num_channels = 4;
    
    _song = song;
#endif
    
    _song = song_from_file(filename);
    
    _song_player_state = create_song_player_state(_song.num_channels);
}

void SynthieHALUnit::GetFrames(Float32 *out, UInt32 numSamples, int sampleRate) {
    if (_first_time) {
        _first_time = false;
    }
    
    song_player_state_t next_state = { 0 };
    double *temp_buffer = new double[numSamples];
    get_song_player_samples(_song, _song_player_state, sampleRate, numSamples, temp_buffer, &next_state);
    
    for (int i=0; i < numSamples; ++i) {
        out[2*i + 0] = temp_buffer[i]; // left
        out[2*i + 1] = temp_buffer[i]; // right
    }
    delete [] temp_buffer;
    
    _song_player_state = next_state;
    
    _sampleIndex += numSamples;
}
