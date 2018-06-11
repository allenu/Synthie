
#include "synthie.h"
#include "test.h"
#include <stdio.h>

void test_song() {
    song_t song = { 0 };

    song.num_instruments = 1;
    song.instruments = new instrument_t[1];

    song.instruments[0].oscillator = kSineWave;
    song.instruments[0].envelope.attack_time = 0.25;
    song.instruments[0].envelope.attack_gain = 1.0;
    song.instruments[0].envelope.decay_time = 0.25;
    song.instruments[0].envelope.sustain_gain = 0.75;
    song.instruments[0].envelope.release_time = 0.5;

    song.num_patterns = 1;
    song.patterns = new pattern_t[1];
    song.patterns[0].bpm = 60.0; // 1 beat per second for testing
    song.patterns[0].num_beats = 4; // 4 beats for this pattern
    song.patterns[0].num_pattern_commands = 0;
    song.patterns[0].pattern_commands = new pattern_command_t[2];
    song.patterns[0].pattern_commands[0].beat_index = 0;
    song.patterns[0].pattern_commands[0].command_type = kPlayTone;
    song.patterns[0].pattern_commands[0].freq = 10.0;
    song.patterns[0].pattern_commands[0].channel = 0;
    song.patterns[0].pattern_commands[0].instrument_index = 0;
    song.patterns[0].pattern_commands[1].beat_index = 2;
    song.patterns[0].pattern_commands[1].command_type = kReleaseTone;
    song.patterns[0].pattern_commands[1].channel = 0;

    song.num_channels = 1;

    double sample_rate = 25;
    song_player_state_t prev_state = create_song_player_state(song.num_channels);
    prev_state.synthesizer_state.num_channels = 1;

    // HACK: To ensure our first sample is at time==0.0, we pretend our previous state's time was -1/sample_rate.
    prev_state.time = - 1.0 / sample_rate;

    // Very low sample rate for testing...
    for (int i=0; i < 100; ++i) {
        song_player_state_t next_state;

        double sample;
        get_song_player_samples(song, prev_state, sample_rate, 1, &sample, &next_state);

        printf("sample %d: %.2f\n", i, sample);

        prev_state = next_state;
    }
}
