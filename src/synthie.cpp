#include "synthie.h"
#include "oscillators.h"
#include <math.h>
#include <assert.h>

// Oscillates from -1 to +1
double f_oscillator_sample(oscillator_t oscillator, double freq, double start_time, double time) {
    double normalized_time = time - start_time;
    double theta = 2.0 * M_PI * freq * normalized_time;

    switch (oscillator) {
        case kSineWave:
            return sin(theta);

        case kTriangleWave:
            return triwave(theta);

        case kSquareWave:
            return squarewave(theta, 0.50);

        case kSawtoothWave:
            return sawtooth(theta);

        case kNoiseWave:
            return noisewave(theta);

        case kSilence:
            return 0.0;

        default:
            assert(false);
    }
}

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

// Get a sample for the given synthesizer state at a given time.
//
// - synthesizer_state: the synthesizer state, which specifies how many channels there are, what instrument they're playing,
//   at what frequency, what envelope they're using, and when the tone started (and possibly ended) playing.
// - time: the time, in seconds, of the sample to get
//
double f_synthesizer_sample(const synthesizer_state_t & synthesizer, double time) {
    // Our "mixing" of audio is super simple: just take the average of all of the channels
    double avg_sample = 0.0;
    for (int i=0; i < synthesizer.num_channels; ++i) {
        const synthesizer_channel_t *channel = &synthesizer.channels[i];
        double instrument_sample = f_instrument_sample(channel->instrument, channel->freq, channel->note_play_time,
                    channel->note_released, channel->note_release_time, time);

        avg_sample += instrument_sample;
    }
    if (synthesizer.num_channels > 0) {
        avg_sample = avg_sample / synthesizer.num_channels;
    }

    return avg_sample;
}

// Given a set of synth commands, update the synth state. The only way to change a synth's state is through
// this function.
//
// - prev_state: the previous synthesizer state
// - num_commands: how many commands to execute on the synth
// - commands: an array of the commands that will change the synth state
//
synthesizer_state_t f_next_synthesizer_state(const synthesizer_state_t & prev_state, 
        int num_commands, 
        const synthesizer_command_t *commands) {

    synthesizer_state_t next_state = prev_state;

    for (int i=0; i < num_commands; ++i) {
        synthesizer_command_t command = commands[i];

        switch (command.command_type) {
            case kPlayTone:
                next_state.channels[command.channel].instrument = command.instrument;
                next_state.channels[command.channel].freq = command.freq;
                next_state.channels[command.channel].note_play_time = command.time;
                next_state.channels[command.channel].note_released = false;
                break;

            case kReleaseTone:
                next_state.channels[command.channel].note_release_time = command.time;
                next_state.channels[command.channel].note_released = true;
                break;

            default:
                assert(false);
        }
    }
    return next_state;
}

// Given a song reader state, move the reader to the new time.
//
// - prev_state: the previous song reader state
// - song: the song being read
// - time: the new time to move the reader to
//
song_reader_state_t f_next_song_reader_state(const song_reader_state_t & prev_state, const song_t & song, double time) {
    song_reader_state_t next_state = prev_state;
    // Pattern commands don't transfer from state to state, so we must clear these out
    next_state.pattern_commands = nullptr;
    next_state.num_pattern_commands = 0;

    pattern_t *prev_pattern = nullptr;
    bool process_next_beat = false;

    if (prev_state.pattern_index == -1) {
        // Special case: when starting playback, pattern_index == -1, which indicates we haven't
        // played any samples yet.
        process_next_beat = true;
    } else {
        prev_pattern = &song.patterns[ prev_state.pattern_index ];
        double prev_beat_period = 60.0 / prev_pattern->bpm;
        double next_beat_time = prev_state.pattern_start_time + prev_beat_period * (prev_state.beat_index + 1);

        process_next_beat = time >= next_beat_time;
    }

    if (process_next_beat) {
        // At end of pattern? Start next one, or repeat entire song if option provided. Set done
        // if not repeating.
        int next_pattern_command_index = prev_state.next_pattern_command_index;
        if (prev_state.pattern_index == -1) {
            // Playback begins now.
            next_state.beat_index = 0;
            next_state.pattern_index = 0;
            next_state.pattern_start_time = time;
            next_pattern_command_index = 0;
        } else if (prev_state.beat_index == prev_pattern->num_beats - 1) {
            double prev_beat_period = 60.0 / prev_pattern->bpm;
            next_state.beat_index = 0;
            next_state.pattern_index++;
            next_state.pattern_start_time = prev_state.pattern_start_time + prev_beat_period * prev_pattern->num_beats;
            next_pattern_command_index = 0;

            if (next_state.pattern_index == song.num_patterns) {
                if (prev_state.song_reader_options.repeats) {
                    next_state.pattern_index = 0;
                } else {
                    next_state.done = true;
                }
            }
        } else {
            // Not done pattern, so move onto next beat.
            next_state.beat_index++;
        }

        if (!next_state.done) {
            pattern_t *curr_pattern = &song.patterns[ next_state.pattern_index ];

            next_state.pattern_commands = nullptr;
            next_state.num_pattern_commands = 0;

            // Find the patterns that apply to this current beat
            while (next_pattern_command_index < curr_pattern->num_pattern_commands
                    && curr_pattern->pattern_commands[ next_pattern_command_index].beat_index < next_state.beat_index ) {
                next_pattern_command_index++;
            }

            if (next_pattern_command_index < curr_pattern->num_pattern_commands
                    && curr_pattern->pattern_commands[ next_pattern_command_index ].beat_index == next_state.beat_index) {
                // We found a pattern command that applies to this beat.
                next_state.pattern_commands = &curr_pattern->pattern_commands[ next_pattern_command_index ];
                
                // Keep going until we go past the commands with the same index
                while (next_pattern_command_index < curr_pattern->num_pattern_commands
                        && curr_pattern->pattern_commands[next_pattern_command_index].beat_index == next_state.beat_index ) {
                    next_pattern_command_index++;
                    next_state.num_pattern_commands++;
                }
            }
            
            next_state.next_pattern_command_index = next_pattern_command_index;
        }
    }

    return next_state;
}

// Convert a set of pattern commands into synthesizer commands. The synthesizer doesn't
// know anything about beat indexes, beat periods or what the instruments are, so this
// translates those concepts into ones that the synthesizer understands.
//
// - num_pattern_commands, pattern_commands: the number of and the pattern commands themselves
// - pattern_start_time: when this pattern began (time of beat 0)
// - beat_period: how long each beat is
// - num_instruments, instruments: the number of and the actual instruments themselves
//
// Caller must delete [] the returned array.
synthesizer_command_t * f_synthesizer_commands_from_pattern_commands(
        int num_pattern_commands, 
        const pattern_command_t *pattern_commands,
        double pattern_start_time,
        double beat_period,
        int num_instruments,
        const instrument_t *instruments) {
    
    synthesizer_command_t *synthesizer_commands = new synthesizer_command_t[num_pattern_commands];
    
    for (int i=0; i < num_pattern_commands; ++i) {
        synthesizer_commands[i].channel = pattern_commands[i].channel;
        synthesizer_commands[i].time = pattern_start_time + pattern_commands[i].beat_index * beat_period;
        synthesizer_commands[i].command_type = pattern_commands[i].command_type;
        if (pattern_commands[i].command_type == kPlayTone) {
            synthesizer_commands[i].instrument = instruments[pattern_commands[i].instrument_index];
            synthesizer_commands[i].freq = pattern_commands[i].freq;
        }
    }
    
    return synthesizer_commands;
}

// Given a song player state, return the next song player state, given the song being played and the current time.
song_player_state_t f_next_song_player_state(
        const song_player_state_t & prev_state, 
        const song_t & song, 
        double time) {

    song_reader_state_t next_song_reader_state = 
        f_next_song_reader_state(prev_state.song_reader_state, song, time);

    synthesizer_state_t next_synthesizer_state;

    if (next_song_reader_state.num_pattern_commands > 0) {
        // We have pattern commands that we must apply to the synthesizer

        const pattern_t *current_pattern = &song.patterns[next_song_reader_state.pattern_index];
        double beat_period = 60.0 / current_pattern->bpm;

        // First convert them to synthesizer commands
        synthesizer_command_t *synthesizer_commands = f_synthesizer_commands_from_pattern_commands(
            next_song_reader_state.num_pattern_commands, 
            next_song_reader_state.pattern_commands,
            next_song_reader_state.pattern_start_time,
            beat_period,
            song.num_instruments,
            song.instruments);

        // Apply them to the synthesizer to get a new state
        next_synthesizer_state = f_next_synthesizer_state(prev_state.synthesizer_state,
            next_song_reader_state.num_pattern_commands,
            synthesizer_commands);

        delete [] synthesizer_commands;
    } else {
        next_synthesizer_state = prev_state.synthesizer_state;
    }

    song_player_state_t next_state;
    next_state.time = time;
    next_state.song_reader_state = next_song_reader_state;
    next_state.synthesizer_state = next_synthesizer_state;

    return next_state;
}

// Create a new song player from scratch.
song_player_state_t create_song_player_state(int num_channels) {
    song_player_state_t song_player_state = { 0 };

    song_player_state.song_reader_state.song_reader_options.repeats = true;
    song_player_state.song_reader_state.pattern_index = -1;
    song_player_state.synthesizer_state.num_channels = num_channels;

    return song_player_state;
}

// Gets the next song player samples, given the previous state and current time. Will also
// output the next song player state.
void get_song_player_samples(
        const song_t & song,
        song_player_state_t prev_state, 
        const double sample_rate,
        const int num_samples,
        double *samples,
        song_player_state_t *next_state) {

    double sample_period = 1.0 / sample_rate;
    
    // Save this since we update prev_state in the loop
    double prev_state_time = prev_state.time;

    for (int i=0; i < num_samples; ++i) {
        if (prev_state.song_reader_state.done) {
            samples[i] = 0.0;
        } else {
            double time = prev_state_time + ((i+1) * sample_period);

            *next_state = f_next_song_player_state(prev_state, song, time);

            samples[i] = f_synthesizer_sample(next_state->synthesizer_state, time);

            // for next loop
            prev_state = *next_state;
        }
    }
    if (prev_state.song_reader_state.done) {
        *next_state = prev_state;
    }
}
