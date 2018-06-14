//
//  AudioHAL.hpp
//  Synthie
//
//  Created by Allen Ussher on 6/2/18.
//  Copyright © 2018 Ussher Press. All rights reserved.
//
//  (Based on code I wrote 10/23/11.)

#ifndef AudioHAL_hpp
#define AudioHAL_hpp

#include <TargetConditionals.h>

#if TARGET_OS_OSX
#include <CoreAudio/AudioHardware.h>
#elif TARGET_OS_IPHONE
#include <CoreAudio/CoreAudioTypes.h>
#include <AudioToolbox/AudioServices.h>
#include <AudioUnit/AudioUnitProperties.h>
#else
#error Platform not supported
#endif

class IAudioHALUnit {
public:
    virtual void GetFrames(Float32 *out, UInt32 numSamples, int sampleRate) = 0;
    virtual ~IAudioHALUnit() {}
};

// How to use:
//
//   auto audioHal = new CAudioHAL();
//
//   auto myAudioUnit = new MyAudioUnit(); // This supports IAudioHALUnit
//   audioHAL->SetAudioUnit(myAudioUnit);
//
//   audioHAL->EnablePlayback();
//
// ...
//
//   audioHAL->DisablePlayback();
//

class CAudioHAL {
public:
    
    CAudioHAL();
    ~CAudioHAL();
    void CreateToneUnit();
    
    void EnablePlayback();
    void DisablePlayback();
#if TARGET_OS_OSX
    void CreateAudioDevice();
    int NumSamplesPerFrame();
#endif
    
    double GetSampleRate();
    
    void SetAudioUnit(IAudioHALUnit * m_audioUnit);
    void GetFrames(Float32 *out, UInt32 numFrames);
    
    
private:
#if TARGET_OS_OSX
    UInt32 m_deviceBufferSize;    // bufferSize returned by kAudioDevicePropertyBufferSize
    AudioStreamBasicDescription    m_deviceFormat;    // info about the default device
    AudioDeviceID    m_device;        // the default device
#elif TARGET_OS_IPHONE
    AudioComponentInstance m_toneUnit;
#endif
    int m_sampleRate;
    bool m_fInitialized;
    bool m_fPlaybackEnabled;
    
    double m_playbackTimeInMs;
    
    IAudioHALUnit * m_audioUnit;
};


#endif /* AudioHAL_hpp */
