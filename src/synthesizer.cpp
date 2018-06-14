#include "synthesizer.h"
#include <math.h>
#include <assert.h>

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

        switch (command.action_type) {
            case kSynthesizerAction_PlayTone:
                next_state.channels[command.channel].instrument = command.instrument;
                next_state.channels[command.channel].freq = command.freq;
                next_state.channels[command.channel].note_play_time = command.time;
                next_state.channels[command.channel].note_released = false;
                break;

            case kSynthesizerAction_ReleaseTone:
                next_state.channels[command.channel].note_release_time = command.time;
                next_state.channels[command.channel].note_released = true;
                break;

            default:
                assert(false);
        }
    }
    return next_state;
}
