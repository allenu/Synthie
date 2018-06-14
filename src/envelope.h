//
// Synthie - Created by Allen Ussher, 2018
//
// If you use this, please credit me somewhere in your app or project.

#pragma once

typedef struct {
    // All times are in seconds
    double attack_time;     // time to reach attack_gain after note played
    double attack_gain;     // max gain we reach at end of attack
    double decay_time;      // time to decay to sustain gain
    double sustain_gain;    // gain level when we are in sustain
    double release_time;    // time to reach 0.0 gain after note released

    // If false, we skip decay/sustain and go directly to release.
    // bool sustainable;
} envelope_t;

double f_envelope_gain(const envelope_t & envelope, bool note_released, double note_release_time, double time);

