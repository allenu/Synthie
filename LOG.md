
# 2018/06/14 Thu

I cleaned up the logic for determining if the song reader state should change. I previously would
force a state change on each sample (inefficient) and in the function that causes a state change
it would check if it was time for the next beat in the pattern to be processed.

This was slightly wasteful and a little hard to read, so instead when a new reader state is 
generated, we calculate the time that the next pattern beat should be processed and use that
to determine if we should update state in the future.

# 2018/06/13 Wed

I've added a platform-agnostic example in examples/generate_pcm_file. Run "make" in the
directory to build, then "make test" to run it, and after installing sox (you'll need
the "play" utility), run "make play".

I broke up the code into their own files instead of having one massive file. Having everything
in one massive file did actually simplify reading the code once you knew how it worked, but I
figured it's easier to explain how it works piece by piece if I actually separated the 
functionality out into their own files.

# 2018/06/10 Sun

Yesterday night I thought about how to define the basic data structures for the music player.
I'm favoring a functional style, so there are only a few moving pieces (i.e. a handful of
mutating data) and data is just transformed via a few functions.

It's early in the morning and I just spent about an hour writing the various structs
and funcs that are needed by Synthie, all in synthie.h and synthie.cpp.

I spent a few hours writing up and testing the majority of the song playback code. Of course,
I had a few bugs along the way. Instead of unit testing, though, I just went into live testing
in the app. :) I have to admit that as long as you know what you're doing, it's quite a bit 
faster than writing individual unit tests for everything.

Anyway, functionally, it works just as I would like. There were some design details that were
a little trickier. Specifically, it took some thinking to work out how to extract the commands
to send to the synthesizer from the last song player state. Eventually I figured out a way to
decouple the two by just having two separate structs for a "pattern command", one that is
produced by the song player and one that is consumed by the synthesizer.

Of course, I'll try to explain all of this in the blog posts!

Some things I'll need to fix up still:
[ ] better bounds checking on everything
[ ] instead of beat_index for pattern events, it should be line_index. I should also specify how many lines per beat as well.
[ ] write a simple script to convert a text file to a json definition of the song
[ ] load a song from json


# 2018/06/09 Sat

I've created a quick blog post and I'm uploading this code to github for the first time.

I'm re-using code from 2011. Unfortunately for the Mac, that means it's using deprecated
APIs. Thankfully, it *still* builds, but it's doubtful Apple would allow such an app that
uses these APIs to be shipped in the Mac App Store.

As far as I can tell, the APIs used for iOS are still kosher.

The low-level audio playback code is old and a little ugly. I want to update it so it's a
little more functional and less object-oriented. However, to bootstrap the project and get
quick results, I'm going to use it as-is for now.

