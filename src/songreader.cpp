
#include "songreader.h"

// Given a song reader state, move the reader to the new time.
//
// - prev_state: the previous song reader state
// - song: the song being read
//
song_reader_state_t f_next_song_reader_state(const song_reader_state_t & prev_state, const song_t & song) {
    song_reader_state_t next_state = prev_state;

    // Pattern commands don't transfer from state to state, so we must clear these out
    next_state.pattern_commands = nullptr;
    next_state.num_pattern_commands = 0;

    pattern_t *prev_pattern = &song.patterns[ prev_state.pattern_index ];
    double tick_period = 60.0 / (prev_pattern->bpm * prev_pattern->ticks_per_beat);

    // At end of pattern? Start next one, or repeat entire song if option provided. Set done
    // if not repeating.
    int next_pattern_command_index = prev_state.next_pattern_command_index;
    if (!prev_state.started_playing) {
        // Special case: We've just started playing, so we're already on the right tick and pattern.
        next_state.started_playing = true;
    } else if (prev_state.tick_index == prev_pattern->ticks_per_pattern - 1) {
        next_state.tick_index = 0;
        next_state.pattern_index++;
        next_state.pattern_start_time = prev_state.pattern_start_time + tick_period * prev_pattern->ticks_per_pattern;
        next_pattern_command_index = 0;

        if (next_state.pattern_index == song.num_patterns) {
            if (prev_state.song_reader_options.repeats) {
                next_state.pattern_index = 0;
            } else {
                next_state.done = true;
            }
        }
    } else {
        // Not done pattern, so move onto next tick.
        next_state.tick_index++;
    }

    if (!next_state.done) {
        pattern_t *curr_pattern = &song.patterns[ next_state.pattern_index ];

        next_state.pattern_commands = nullptr;
        next_state.num_pattern_commands = 0;

        // Find the patterns that apply to this current tick
        while (next_pattern_command_index < curr_pattern->num_pattern_commands
                && curr_pattern->pattern_commands[ next_pattern_command_index].tick_index < next_state.tick_index ) {
            next_pattern_command_index++;
        }

        if (next_pattern_command_index < curr_pattern->num_pattern_commands
                && curr_pattern->pattern_commands[ next_pattern_command_index ].tick_index == next_state.tick_index) {
            // We found a pattern command that applies to this tick.
            next_state.pattern_commands = &curr_pattern->pattern_commands[ next_pattern_command_index ];
            
            // Keep going until we go past the commands with the same index
            while (next_pattern_command_index < curr_pattern->num_pattern_commands
                    && curr_pattern->pattern_commands[next_pattern_command_index].tick_index == next_state.tick_index ) {
                next_pattern_command_index++;
                next_state.num_pattern_commands++;
            }
        }
        
        next_state.next_pattern_command_index = next_pattern_command_index;
    }

    return next_state;
}
