
#include <stdio.h>
#include "songreader.h"

typedef struct __attribute__ ((packed)) {
   __uint8_t oscillator;
   __uint8_t padding[3];

   double attack_time;
   double attack_gain;
   double decay_time;
   double sustain_gain;
   double release_time;
} instrument_representation_t;

typedef struct __attribute__ ((packed)) {
    __int8_t tick_index;
    __int8_t instrument_index;
    __int8_t channel;
    __int8_t action_type; // 0 == play, 1 == release
    __int8_t note;
    __int8_t octave;
} pattern_command_representation_t;

typedef struct __attribute__ ((packed)) {
    __uint16_t num_commands;
} song_pattern_header_t;

typedef struct __attribute__ ((packed)) {
    char signature[4];  // 'SNG\0'
    __uint16_t major_version;
    __uint16_t minor_version;
    double bpm;
    __uint16_t ticks_per_beat;
    __uint16_t num_instruments;
    __uint16_t num_patterns;
    __uint16_t ticks_per_pattern;
} song_header_t;


// Octave first, then note
double s_AllNotes[12] = {16.35, 17.32, 18.35, 19.45,
    20.60,21.83,23.12,24.50,
    25.96,27.50,29.14,30.87};

double GetToneForNote(__int8_t note, __int8_t octave) {
    return s_AllNotes[note] * ( 1 << octave );
}

/* File format:
 *
 * - song header
 * - num_instruments x instrument_representation
 * - num_patterns x (pattern header + (num_commands x pattern_command))
 */
song_t song_from_file(const char *filename) {
    song_t song = { 0 };

    // TODO:
    song.num_channels = 4;

    FILE *fp = fopen(filename, "rb");
    if (fp) {
        song_header_t song_header;
        fread(&song_header, sizeof(song_header_t), 1, fp);

        song.num_instruments = song_header.num_instruments;
        song.instruments = new instrument_t[song_header.num_instruments];
        song.num_patterns = song_header.num_patterns;
        song.patterns = new pattern_t[song_header.num_patterns];

        for (int i=0; i < song_header.num_instruments; ++i) {
            instrument_representation_t instrument;
            fread(&instrument, sizeof(instrument_representation_t), 1, fp);

            printf("sizeof rep %d\n", sizeof(instrument_representation_t));
            printf("oscillator %d\n", instrument.oscillator);

            song.instruments[i].oscillator = static_cast<oscillator_t>(instrument.oscillator);
            song.instruments[i].envelope.attack_time = instrument.attack_time;
            song.instruments[i].envelope.attack_gain = instrument.attack_gain;
            song.instruments[i].envelope.decay_time = instrument.decay_time;
            song.instruments[i].envelope.sustain_gain = instrument.sustain_gain;
            song.instruments[i].envelope.release_time = instrument.release_time;

            printf("instrument %d: %d    %.2f %.2f  %.2f %.2f  %.2f\n",
                    i,
                    song.instruments[i].oscillator,
                    song.instruments[i].envelope.attack_time,
                    song.instruments[i].envelope.attack_gain,
                    song.instruments[i].envelope.decay_time,
                    song.instruments[i].envelope.sustain_gain,
                    song.instruments[i].envelope.release_time);
        }

        for (int i=0; i < song_header.num_patterns; ++i) {
            song_pattern_header_t header;
            fread(&header, sizeof(song_pattern_header_t), 1, fp);

            song.patterns[i].bpm = song_header.bpm;
            song.patterns[i].ticks_per_beat = song_header.ticks_per_beat;
            song.patterns[i].ticks_per_pattern = song_header.ticks_per_pattern;
            song.patterns[i].num_pattern_commands = header.num_commands;

            song.patterns[i].pattern_commands = new pattern_command_t[header.num_commands];
            for (int j=0; j < header.num_commands; ++j) {
                pattern_command_representation_t command;
                fread(&command, sizeof(pattern_command_representation_t), 1, fp);

                song.patterns[i].pattern_commands[j].tick_index = command.tick_index;
                song.patterns[i].pattern_commands[j].action_type = static_cast<pattern_action_t>(command.action_type);

                printf("command %d: tick_index %d   ins %d  note: %d - %d\n", 
                        j,
                        command.tick_index,
                        command.instrument_index,
                        command.note, command.octave);

                double freq = GetToneForNote(command.note, command.octave);
                song.patterns[i].pattern_commands[j].freq = freq;
                song.patterns[i].pattern_commands[j].channel = command.channel;
                song.patterns[i].pattern_commands[j].instrument_index = command.instrument_index;
            }
        }

        fclose(fp);
    }
    return song;
}

