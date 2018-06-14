# TODO:

- [ ] clean-up
    - [ ] get rid of process_next_beat flag and pattern_start_time
        - we can increment the reader to the next pattern line command manually
        - just use bpm
    - [ ] separate synthesizer code from song player
    - [ ] separate song player sample provider from the synthesizer and song player code
        it's better to think of the combination of synth and song player as separate from the other two
    - [ ] bounds-checking
- [ ] update old script that converted text file to a .plist and instead dump out a json file

- [ ] support notes that release immediately
- [ ] change beat_index to line_index; add lines_per_beat to pattern
- [ ] support gain per channel
- [ ] support gain change command
- [ ] support phase change

- [x] Write platform-agnostic test that spits out a PCM file that you can play with sox
- [ ] Add simple tone playback for Raspberry Pi
- [ ] Clean up AudioHAL so it's more functional and uses less mutable data

- [x] get simple sine tone working on Mac again
- [x] get simple sine tone working on iOS again

- [x] Define structures

- [x] Play tones
    - [x] Set up oscillator code
        - [x] write sin, triangle, sawtooth, pulse waves
    - [x] set up as envelope
    - [x] play tone at 0s

- [x] Play timed tones
    - [x] once every 1s on the same channel


- [x] Support more than one channel


