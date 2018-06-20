# makesong.rb and song2pcm

These are two simple tools that let you write songs using a simple text format and then
convert them to PCM files for playback with SOX. The files generated are raw 8-bit unsigned
PCM audio files at 44.1kHz.

You can play a raw PCM file of this format like this, assuming the file is output.pcm:

   play -t raw -r 44100 -e unsigned-integer -b 8 -c 1 output.pcm

Of course, you can use whatever you want (for instance, Audacity) to play these files, as
long they are able to play raw PCM.

## Usage

To convert a text file into a binary. song file that song2pcm can play:

    ./makesong.rb input_song.txt output_file.song

After this, use song2pcm to convert it to a 44.1kHz PCM file:

    ./song2pcm output_file.song output.pcm


# Song file format

The song file you write the song with is just a standard text file. See example_song.txt
for a good example.

Each section starts with a label that starts with a dot. There are three types of sections
at the moment: .track_info, .instruments, and .pattern.

A song is made up of one or more patterns, where each pattern describes which note is played
at a given time, using ticks.

## .track_info

You can set beats per minute (bpm) here, as well as ticks_per_beat and ticks_per_pattern.

A beat is a note (usually a quarter note in 4/4 time). ticks_per_beat allows you to break
up a note into more than one tick to get things like eigth or sixteenth notes. For simplicity,
you can just set ticks_per_beat to 1 so that 1 beat == 1 tick. To allow for eigth notes,
use ticks_per_beat: 2, and for sixteenth notes use ticks_per_beat: 4.

ticks_per_pattern dictates how long a "pattern" is in ticks.

## .instruments

This section describes one or more instruments. Each instrument follows this format.

instrument_name: <waveform>   <attack_time> <attack_gain>  <decay_time> <sustain_gain> <release_time>

instrument_name can be whatever you want, so long as it's a string of non-whitespace characters.

waveform is one of the following
    silence
    sine
    triangle
    square
    sawtooth
    noise

All times are in seconds, but can be fractional (ex: 0.10 for 100ms). Gains are in the range 0.0 to 1.0.

## .pattern

A pattern is labeled like this:

.pattern <index>

where <index> is 0 or greater. At the moment it's not used, but will be in the future.

A pattern is made up of one or more lines of this form:

    <tick_index>: <channel_num> <instrument_name> <note and octave>

A line is just a command to play a note (or release a note).

tick_index ranges from 0 to (ticks_per_pattern-1) and tells the song player when to play the note.
channel_num, for now, must be from 0 to 3. You have 4 channels available to play notes on.
note and octave is a combination of the note name and the octave, like this
    c5
    d#4
    g#1
    f3

Note that only sharp notation is supported at the moment (i.e. use a# and not bb).

You can also repeat the last note on a channel using "." instead of providing the instrument name and note:

    0: 0 bass c5
    1: 0 .
    2: 0 .
    3: 0 .

To release a note, use "----" on a channel:

    0: 0 bass a#4
    4: 0 ----

You can write comments in the song by starting a line with a pound character.

Note that the lines you provide do not have to be in the order in which they are played.
All of the lines will be sorted based on the first field, the tick index. This allows you
to break up a pattern into logical sections that are easier to read. They'll be played in
order of the tick index.

For instance:

    # some bass line
    0: 0 bass c3
    1: 0 bass e3
    2: 0 bass g3
    3: 0 bass c3
    4: .
    5: .
    6: .
    7: .

    # some melody
    0: 1 violin e5
    2: 1 violin f5
    3: 1 vioiln f6

Will get merged and all lines starting with "0:" get played first, then lines with "1:", 
then "2:" and so on...


# Installing SOX

You can grab SOX here: http://sox.sourceforge.net/

If you're on a mac, you can use brew to install it

    brew install sox

If you're running Debian Linux or a variant, just do

    sudo apt-get install sox

