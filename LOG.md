
# 2018/06/10 Sun

Yesterday night I thought about how to define the basic data structures for the music player.
I'm favoring a functional style, so there are only a few moving pieces (i.e. a handful of
mutating data) and data is just transformed via a few functions.

It's early in the morning and I just spent about an hour writing the various structs
and funcs that are needed by Synthie, all in synthie.h and synthie.cpp.

# 2018/06/09 Sat

I've created a quick blog post and I'm uploading this code to github for the first time.

I'm re-using code from 2011. Unfortunately for the Mac, that means it's using deprecated
APIs. Thankfully, it *still* builds, but it's doubtful Apple would allow such an app that
uses these APIs to be shipped in the Mac App Store.

As far as I can tell, the APIs used for iOS are still kosher.

The low-level audio playback code is old and a little ugly. I want to update it so it's a
little more functional and less object-oriented. However, to bootstrap the project and get
quick results, I'm going to use it as-is for now.

