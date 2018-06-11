typedef enum { 
    kSilence = 0,
    kSineWave,
    kTriangleWave,
    kSquareWave,
    kSawtoothWave,
} oscillator_t;

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

typedef struct {
    oscillator_t oscillator;
    envelope_t envelope;
} instrument_t;

typedef struct {
    instrument_t instrument;
    double freq;

    // TODO:
//    double phase;
//    double gain_factor;
    double note_play_time;
    bool note_released;
    double note_release_time;
} synthesizer_channel_t;

#define MAX_CHANNELS 16

typedef struct {
    int num_channels;
    // TODO: avoid a fixed size here?
    synthesizer_channel_t channels[MAX_CHANNELS];
} synthesizer_state_t;

typedef enum {
    kPlayTone = 0,
    kReleaseTone,
} pattern_command_type_t;

typedef struct {
    int beat_index;
    pattern_command_type_t command_type;
    double freq;

    int channel;
    int instrument_index;
} pattern_command_t;

typedef struct {
    int channel;
    double time;
    pattern_command_type_t command_type;

    // These are only used during PlayTone commands
    instrument_t instrument;
    double freq;
} synthesizer_command_t;

typedef struct {
    double bpm;
    int num_beats;  // How long the pattern is in beats

    int num_pattern_commands;
    pattern_command_t *pattern_commands;
} pattern_t;

typedef struct {
    int num_instruments;
    instrument_t *instruments;

    int num_patterns;
    pattern_t *patterns;

    int num_channels;
} song_t;

typedef struct {
    bool repeats;
    double delay_between_repetition;
    // double gain;
} song_playback_options_t;

typedef struct {
    song_playback_options_t song_playback_options;
    bool done;

    double pattern_start_time; // when we started playing this pattern
    int pattern_index;
    int beat_index;

    // Cache of the index of the next command to execute in the pattern.
    // This is just an optimization so that we don't search through the
    // pattern_commands to find the next command with the same beat_index
    // as we need.
    int next_pattern_command_index;

    // Which commands to execute for this beat. Is just a pointer into 
    // pattern_commands for the given pattern.
    int num_pattern_commands;
    pattern_command_t *pattern_commands;
} song_playback_state_t;

typedef struct {
    double time;
    song_playback_state_t song_playback_state;
    synthesizer_state_t synthesizer_state;
} song_player_state_t;

// API
song_player_state_t create_song_player_state(int num_channels);

void get_song_player_samples(
        const song_t & song,
        song_player_state_t prev_state, 
        const double sample_rate,
        const int num_samples,
        double *samples,
        song_player_state_t *next_state);
