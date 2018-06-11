//
//  SynthieHALUnit.cpp
//  Synthie
//
//  Created by Allen Ussher on 6/10/18.
//  Copyright © 2018 Ussher Press. All rights reserved.
//

#include "SynthieHALUnit.hpp"

SynthieHALUnit::SynthieHALUnit() {
    song_t song = { 0 };
    
    song.num_instruments = 1;
    song.instruments = new instrument_t[1];
    
    song.instruments[0].oscillator = kSineWave;
    song.instruments[0].envelope.attack_time = 0.1;
    song.instruments[0].envelope.attack_gain = 1.0;
    song.instruments[0].envelope.decay_time = 0.05;
    song.instruments[0].envelope.sustain_gain = 0.75;
    song.instruments[0].envelope.release_time = 0.2;
    
    song.num_patterns = 1;
    song.patterns = new pattern_t[1];
    song.patterns[0].bpm = 120.0;
    song.patterns[0].num_beats = 4;
    song.patterns[0].num_pattern_commands = 4;
    song.patterns[0].pattern_commands = new pattern_command_t[4];
    song.patterns[0].pattern_commands[0].beat_index = 0;
    song.patterns[0].pattern_commands[0].command_type = kPlayTone;
    song.patterns[0].pattern_commands[0].freq = 440.0;
    song.patterns[0].pattern_commands[0].channel = 0;
    song.patterns[0].pattern_commands[0].instrument_index = 0;
    song.patterns[0].pattern_commands[1].beat_index = 0;
    song.patterns[0].pattern_commands[1].command_type = kPlayTone;
    song.patterns[0].pattern_commands[1].freq = 350.0;
    song.patterns[0].pattern_commands[1].channel = 1;
    song.patterns[0].pattern_commands[2].instrument_index = 0;
    song.patterns[0].pattern_commands[2].beat_index = 3;
    song.patterns[0].pattern_commands[2].command_type = kReleaseTone;
    song.patterns[0].pattern_commands[2].channel = 0;
    song.patterns[0].pattern_commands[3].instrument_index = 0;
    song.patterns[0].pattern_commands[3].beat_index = 3;
    song.patterns[0].pattern_commands[3].command_type = kReleaseTone;
    song.patterns[0].pattern_commands[3].channel = 1;

    song.num_channels = 2;
    
    _song = song;
    
    _song_player_state = create_song_player_state(song.num_channels);
}

void SynthieHALUnit::GetFrames(Float32 *out, UInt32 numSamples, int sampleRate) {
    if (_first_time) {
        _first_time = false;
        _song_player_state.time = - 1.0 / (double)sampleRate;
    }
    
    song_player_state_t next_state = { 0 };
    double *temp_buffer = new double[numSamples];
    get_song_player_samples(_song, _song_player_state, sampleRate, numSamples, temp_buffer, &next_state);
    
    for (int i=0; i < numSamples; ++i) {
        out[2*i + 0] = temp_buffer[i]; // left
        out[2*i + 1] = temp_buffer[i]; // right
    }
    delete [] temp_buffer;
    
    if (next_state.song_playback_state.beat_index == 0) {
        printf("%.2f: %.2f\n", next_state.time, temp_buffer[0]);
    }
    
    _song_player_state = next_state;
    
    _sampleIndex += numSamples;
}
