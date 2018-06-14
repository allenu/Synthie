#include "instrument.h"

// Return sample for a given instrument at a given time.
//
// - instrument: which instrument (defines oscillator and envelope)
// - freq: frequency of the tone
// - note_play_time: when the note started playing
// - note_released: true if note was released at some point
// - note_release_time: time of when note was released (only applies if note_released is true)
// - time: time in seconds of the sample to get
//
double f_instrument_sample(const instrument_t & instrument, double freq, double note_play_time, bool note_released, double note_release_time, double time) {
    double oscillator_sample = f_oscillator_sample(instrument.oscillator, freq, note_play_time, time);

    // Translate time into a time in the envelope's timeline (where time 0.0 is where the note starts playing).
    double envelope_time = time - note_play_time;
    double envelope_note_release_time = note_release_time - note_play_time;
    double envelope_gain = f_envelope_gain(instrument.envelope, note_released, envelope_note_release_time, envelope_time);

    double sample = oscillator_sample * envelope_gain;
    return sample;
}
