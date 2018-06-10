
typedef enum { 
    kSineWave = 0,
    kTriangleWave,
    kSquareWave,
    kSawtoothWave,
} oscillator_t;

typedef struct {
    double attack_time;
    double attack_gain;
    double decay_time; // time to decay to sustain
    double sustain_gain;
    double release_time;

    // If false, we skip decay/sustain and go directly to release.
    bool sustainable;
} envelope_t;

typedef struct {
    oscillator_t oscillator;
    envelope_t envelope;
} instrument_t;

typedef struct {
    instrument_t instrument;
    double gain_factor;
    double start_time;
    double release_time;
} synth_channel_t;

typedef struct {
    int num_channels;
    synth_channel_t *channels;

    double sample_rate;
    size_t sample_index;
} synthesizer_state_t;


typedef enum {
    kPlayTone = 0,
    kReleaseTone,
} pattern_command_type_t;

typedef struct {
    int beat_index;
    pattern_command_type_t command_type;

    int instrument_index;
    int channel_index;
} pattern_command_t;

typedef struct {
    double bpm;
    int num_beats;  // How long the pattern is in beats

    size_t num_pattern_commands;
    pattern_command_t *pattern_commands;
} pattern_t;

typedef struct {
    size_t num_instruments;
    instrument_t *instruments;

    size_t num_patterns;
    pattern_t *patterns;

    int num_channels;
} song_t;

typedef struct {
    bool repeat;
    bool done;

    double time;
    
    // The time the current beat ends and a new one starts.
    // We'll use this to see if we should update the song playback
    // state.
    double next_beat_time; 


    double pattern_start_time; // when we started playing this pattern
    int pattern_index;
    int beat_index;

    // Cache of the index of the next command to execute in the pattern.
    // This is just an optimization so that we don't search through the
    // pattern_commands to find the next command with the same beat_index
    // as we need.
    int next_pattern_commands_index;

    // Which commands to execute for this beat. Is just a pointer into 
    // pattern_commands for the given pattern.
    int num_pattern_commands;
    pattern_command_t *pattern_commands;
} song_playback_state_t;

typedef struct {
    song_playback_state_t song_playback_state;
    synthesizer_state_t synthesizer_state;
} song_player_state_t;
