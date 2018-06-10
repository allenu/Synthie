
#include "synthie.h"

double tone_sample(oscillator_t oscillator, freq, double start_time, double time) {
    // convert time to theta using freq

    return sin(theta);
}

double envelope_sample(envelope_t envelope, double start_time, double release_time, double time) {
    // Determine where we are in ADSR
    // Calculate gain based on A-D, or D-S, or S, or R
}

double instrument_sample(instrument_t instrument, double start_time, double release_time, double time) {
}

double synthesizer_sample(synthesizer_state_t synthesizer, double time) {
}

synthesizer_state_t next_synthesizer_state(synthesizer_state_t prev_state, pattern_t pattern, double time) {
}

song_state_t next_song_state(song_state_t prev_state, double time) {
    song_state_t next_state;

    next_state.time = time;

    // See if we finished this pattern.
    // If so, update pattern index, beat_index.
    // See if done song. If so, set done. Return early.
    // If not end of pattern, increment beat as necessary.
    // If beat incremented, process all next pattern commands on this beat. Update synthesizer_state

    return next_state;
}

song_player_state_t next_song_player_state(song_player_state_t prev_state, double time) {
    // get next song state
    // get next synthesizer state
    // from next song state, overwrite synthesizer state as needed with commands found
}

// Gets the next song player samples, given the previous state and current time. Will also
// output the next song player state.
void get_song_player_samples(const song_player_state_t & prev_state, 
        const double sample_rate,
        const int num_samples,
        double *samples,
        song_player_state_t *next_state) {
}

