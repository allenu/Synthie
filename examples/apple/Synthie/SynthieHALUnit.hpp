//
//  SynthieHALUnit.hpp
//  Synthie
//
//  Created by Allen Ussher on 6/10/18.
//  Copyright © 2018 Ussher Press. All rights reserved.
//

#ifndef SynthieHALUnit_hpp
#define SynthieHALUnit_hpp

#include "AudioHAL.hpp"
#include "synthie.h"

class SynthieHALUnit: public IAudioHALUnit {
public:
    SynthieHALUnit(const char *filename);
    virtual ~SynthieHALUnit() {}

    void GetFrames(Float32 *out, UInt32 numSamples, int sampleRate);

private:
    int _sampleIndex = 0;
    song_t _song;
    song_player_state_t _song_player_state;
    bool _first_time = true;
};

#endif /* SynthieHALUnit_hpp */
