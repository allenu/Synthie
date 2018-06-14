#include "envelope.h"

// Return the gain (from 0.0 to 1.0) for a given time within an envelope.
//
// - envelope: which envelope to use for ADSR data
// - note_released: is true if the note was released at some point
// - note_release_time: only applies if note_released is true
// - time: time on the envelope timeline, where 0.0 is the start of attack, in seconds
// 
double f_envelope_gain(const envelope_t & envelope, bool note_released, double note_release_time, double time) {
    if (time < 0.0) {
        // Hasn't started playing yet
        return 0.0;
    }

    // See if we're in the attack phase
    if (time < envelope.attack_time) {
        double gain = envelope.attack_gain * time / envelope.attack_time;
        return gain;
    }

    // Normalize to start of decay time, which is the end of attack_time
    double decay_time = time - envelope.attack_time;

    // See if we're in the decay phase
    if (decay_time < envelope.decay_time) {
        double gain_drop = (envelope.attack_gain - envelope.sustain_gain);
        double gain = envelope.attack_gain - gain_drop * (decay_time / envelope.decay_time);
        return gain;
    }

    // If we got past decay phase, we must either be sustaining, releasing, or no longer playing
    // TODO: We'll support non-sustainable later. Assume everything
    // sustains for now.

    // See if we're still in the sustain phase
    if (!note_released || time < note_release_time) {
        return envelope.sustain_gain;
    }

    // If got here, we must be releasing or no longer playing (post-release)

    // Normalize to release time
    double release_time = time - note_release_time;

    // See if we're still releasing
    if (release_time < envelope.release_time) {
        double gain_drop = envelope.sustain_gain;
        double gain = envelope.sustain_gain - gain_drop * (release_time / envelope.release_time);
        return gain;
    }

    // We must be in post-release phase
    return 0.0;
}

