#include "synthie.h"
#include "song_from_file.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage:\n\n  ./songplayer <input.song> <output.pcm>\n\n");
        return -1;
    }
    const char *input_filename = argv[1];
    const char *output_filename = argv[2];

    song_t song = song_from_file(input_filename);

    printf("num instruments: %d\n", song.num_instruments);
    printf("num patterns: %d\n", song.num_patterns);
    printf("num channels: %d\n", song.num_channels);
    printf("bpm: %.2f\n", song.patterns[0].bpm);
    printf("ticks per pattern: %d\n", song.patterns[0].ticks_per_pattern);

    printf("pattern 0 commands: %d\n", song.patterns[0].num_pattern_commands);

    song_player_state_t prev_state = create_song_player_state(song.num_channels);

    const double sample_rate = 44100;
    const double num_seconds = 20.0;
    const double total_samples = num_seconds * sample_rate;

    FILE *fp = fopen(output_filename, "wb");
    if (!fp) {
        printf("Couldn't write to output file %s\n", output_filename);
        return -1;
    }

    for (int i=0; i < total_samples; ++i) {
        song_player_state_t next_state;

        double sample;
        get_song_player_samples(song, prev_state, sample_rate, 1, &sample, &next_state);

        __uint8_t byte_sample = (sample * 127.0) + 127;
        fwrite(&byte_sample, sizeof(byte_sample), 1, fp);

        prev_state = next_state;
    }

    fclose(fp);

    return 0;
}
