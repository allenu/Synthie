
#include "synthie.h"
#include <stdio.h>

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
    { kSquareWave,   { 0.01, 0.2, 0.01, 0.3, 0.5 } },
    { kTriangleWave, { 0.025, 1.0, 0.025, 0.8, 0.5 } },
    { kNoiseWave,    { 0.025, 0.7, 0.025, 0.5, 0.05 } },
};

#define Bass_Instrument 0
#define Horn_Instrument 1
#define Snare_Instrument 2

static pattern_command_t s_pattern0_commands[] = {
    pattern_command_t { 0, kPlayTone, Note_C, 0, Bass_Instrument },
    pattern_command_t { 0, kPlayTone, Note_C, 1, Horn_Instrument },
    pattern_command_t { 1, kPlayTone, Note_E, 2, Horn_Instrument },
    pattern_command_t { 2, kPlayTone, Note_C, 0, Bass_Instrument },
    pattern_command_t { 2, kPlayTone, Note_C, 3, Snare_Instrument },
    pattern_command_t { 2, kReleaseTone, 0.0, 1, 0 },
    pattern_command_t { 3, kReleaseTone, 0.0, 2, 0 },
    pattern_command_t { 3, kReleaseTone, 0.0, 3, 0 },
    pattern_command_t { 4, kPlayTone, Note_F, 0, Bass_Instrument },
    pattern_command_t { 4, kPlayTone, Note_F, 1, Horn_Instrument },
    pattern_command_t { 5, kPlayTone, Note_A, 2, Horn_Instrument },
    pattern_command_t { 6, kPlayTone, Note_F, 0, Bass_Instrument },
    pattern_command_t { 6, kPlayTone, Note_C, 3, Snare_Instrument },
    pattern_command_t { 7, kReleaseTone, 0.0, 1, 0 },
    pattern_command_t { 7, kReleaseTone, 0.0, 2, 0 },
    pattern_command_t { 7, kReleaseTone, 0.0, 3, 0 },
};
static pattern_t s_patterns[] = {
    { 120.0, 8, sizeof(s_pattern0_commands)/sizeof(s_pattern0_commands[0]), s_pattern0_commands },
};


song_t create_song() {
    song_t song = { 0 };
    
    song.num_instruments = sizeof(s_instruments)/sizeof(s_instruments[0]);
    song.instruments = s_instruments;
    
    song.num_patterns = sizeof(s_patterns)/sizeof(s_patterns[0]);
    song.patterns = s_patterns;
    song.num_channels = 4;

    return song;
}

int main(int argc, char *argv[]) {
    song_t song = create_song();

    song_player_state_t prev_state = create_song_player_state(song.num_channels);

    const double sample_rate = 44100;
    const double num_seconds = 10.0;
    const double total_samples = num_seconds * sample_rate;

    // HACK: To ensure our first sample is at time==0.0, we pretend our previous state's time was -1/sample_rate.
    prev_state.time = - 1.0 / sample_rate;

    FILE *fp = fopen("output.pcm", "wb");

    for (int i=0; i < total_samples; ++i) {
        song_player_state_t next_state;

        double sample;
        get_song_player_samples(song, prev_state, sample_rate, 1, &sample, &next_state);

        __uint8_t byte_sample = (sample * 127.0) + 127;
        fwrite(&byte_sample, sizeof(byte_sample), 1, fp);

        prev_state = next_state;
    }

    fclose(fp);

    return 0;
}
