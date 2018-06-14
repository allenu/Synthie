#include "songplayer.h"
#include <math.h>
#include <assert.h>

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
        switch (pattern_commands[i].action_type) {
        case kPatternAction_PlayTone:
            synthesizer_commands[i].action_type = kSynthesizerAction_PlayTone;
            break;

        case kPatternAction_ReleaseTone:
            synthesizer_commands[i].action_type = kSynthesizerAction_ReleaseTone;
            break;

        default:
            assert(false);
        }
        if (pattern_commands[i].action_type == kPatternAction_PlayTone) {
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

        // TODO: 
        // here we check if the time delta is greater than when the next scheduled pattern line should be processed
        // if so, read the next pattern line
        // add beat_period to the previous time to get the next one
        // if this is the start of a pattern, record that

        // First convert them to synthesizer commands
        synthesizer_command_t *synthesizer_commands = f_synthesizer_commands_from_pattern_commands(
            next_song_reader_state.num_pattern_commands, 
            next_song_reader_state.pattern_commands,
            // TODO: use the pattern start time here
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
