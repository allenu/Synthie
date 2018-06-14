# Synthie

Synthie is an 8-bit style synth audio/music player. It's a work in progress.

For more information, read blog posts here: 

  https://www.ussherpress.com/blog/?tag=synthie

This README file will be updated as the blog posts come out and as new functionality is added.

# Try it out

After cloning:

    cd examples/generate_pcm_file
    make
    make test
    make play

You'll need sox installed to run the "make play" part.

# Design

From the lowest level to the highest level, Synthie is made up of the following parts:

- oscillators
- envelopes
- instruments
- synthesizer
- a song
- song reader
- song player

Synthie is written in a functional programming style. There are really two main structures that maintain state, the
synthesizer representation and the song reader. (There is a third, the song player, but it's just made up of the
synthesizer and song reader.)

As such, the state of the synthesizer is changed via a function. The song reader also changes via a single function
call. In other words, the data itself never mutates. It's merely transformed from the old state to the new one in
one lock step.

A description of each part follows.

## Oscillators
These are what generate the tones. Synthie plays the following types of tones:

- silence
- sine wave
- triangle wave
- sawtooth wave
- square wave
- random noise

## Envelopes
An envelope defines the gain (i.e. volume) of an oscillation over time. They are made up attack, decay, sustain,
and release phases (ADSR). [1]

[1] See https://en.wikipedia.org/wiki/Synthesizer#Attack_Decay_Sustain_Release_(ADSR)_envelope

## Instruments
An instrument is a combination of an envelope and an oscillator played at specific frequency and time.

## Synthesizer
The synthesizer object merely represents one or more "channels" that each play one instrument at a time. In other
words, it's a snapshot in time of the current state of the synthesizer.

Each channel stores:
  - what oscillator is playing at what frequency
  - what envelope is used
  - what time the oscillator and envelope started

A synthesizer doesn't "generate" sound itself, but you can extract a sound sample from a synthesizer given 
an input time via the f_instrument_sample() function. Using the above information for each channel, the sample
at time t can be deduced. Each sample from each channel is then mixed simply by averaging them to ensure no
overflow, which would lead to clipping of the audio.

## Song
A song is simply a list of instructions about which notes to play at what time. More specifically, the song
structure is made up of an array of patterns, where each pattern is made up of lines, each line corresponding to 
a specific moment in time (think of it as a beat). (In fact, each pattern is assigned a specific beats per minute
value so that the song player knows when to play each beat.) 

Each line in a pattern can have zero or more commands for each synthesizer channel. At the moment, a command is 
either "play a tone on channel n at a given frequency f using instrument y" or "release the tone that was playing
on channel n".

## Song Reader
The song reader allows a song to be played "line by line". As with the synthesizer, a song reader is transformed from one state to another. It is not mutable. The song reader
is transformed whenever the next line needs to read.

## Song Player
The song player is simply the combination of the two states: the synthesizer object and the song reader, along with
a timestamp of the last sample that was requested.

Call get_song_player_samples() on a song player struct to get n audio samples from it. This has the effect of updating
both the song reader and synthesizer states.


# Some quick background

In 2011, I decided to write a synth music player for a retro game engine I was working on at the time.

I got GoatTracker music playing. Here's a demo: https://www.youtube.com/watch?v=UEUSqySKMCU
In 2013, I put together a simple piano player app: https://www.youtube.com/watch?v=iymaYUtI9_o
 
In 2018, I decided to write this again from the ground up for education purposes. I'll be updating
my blog to explain the process (see link above). 

The code will be usable on any platform, so long as you have a way to play PCM audio samples. In
this project I'll be targeting macOS, iOS, and Raspberry Pi (why not) and possibly Arduino as well
(also why not).


# TODOs

For most of my personal projects, I keep track of tasks using a simple TODO markdown file. Open up
TODO.md to see what upcoming tasks I've created for myself.

I'll try to maintain a LOG.md file as well with notes on what the next steps will be.


# Social Media

Follow me on twitter [@ussherpress](https://twitter.com/ussherpress)
