//
//  AudioHAL.cpp
//  Synthie
//
//  Created by Allen Ussher on 6/2/18.
//  Copyright © 2018 Ussher Press. All rights reserved.
//
//  (Based on code I wrote 10/23/11.)
//
//  NOTE: On Mac, this uses CoreAudio APIs that were deprecated in OS X 10.6. A future work item is
//  to update this to support more modern APIs.

#include "TargetConditionals.h"

#if TARGET_OS_IPHONE
#import <AudioUnit/AudioUnit.h>
#import <AudioToolbox/AudioToolbox.h>
#endif

#include "AudioHAL.hpp"

#define DEFAULT_SAMPLE_RATE  22050

class CToneUnit : public IAudioHALUnit {
public:
    
    CToneUnit() : m_playbackTimeInMs(0) {}
    
    void GetFrames(Float32 *out, UInt32 numSamples, int sampleRate) {
        // Figure out wavelength in ms
        double oscillator_freq = 440.0;
        double other_oscillator_freq = 350.0;
        double wavelength = 1000.0 / oscillator_freq;   // wave length in ms
        double msPerFrame = (1000.0 / sampleRate);    // sample frame length in ms
        
        // Figure out where in the wavelength we are. Likely an easier way to do this.
        double timeInWavelength = m_playbackTimeInMs - wavelength * ((int)(m_playbackTimeInMs / wavelength));
        double theta = 2. * M_PI * timeInWavelength / wavelength;
        
        double theta_per_frame = 2. * M_PI * msPerFrame / wavelength;

        // Figure out where in the wavelength we are. Likely an easier way to do this.
        double secondWavelength = 1000.0 / other_oscillator_freq;   // wave length in ms
        double secondTimeInWavelength = m_playbackTimeInMs - secondWavelength * ((int)(m_playbackTimeInMs / secondWavelength));
        double second_theta = 2. * M_PI * secondTimeInWavelength / secondWavelength;
        
        double second_theta_per_frame = 2. * M_PI * msPerFrame / secondWavelength;
        

        for (int i=0; i<numSamples; i++)
        {
            if (theta > 2. * M_PI)
                theta -= (2. * M_PI);
            if (second_theta > 2. * M_PI)
                second_theta -= (2. * M_PI);

            Float32 sample = (sin(theta) + sin(second_theta))/2.0;
            
            out[i*2 + 0] = sample;    // left channel
            out[i*2 + 1] = sample;    // right channel
            
            theta += theta_per_frame;
            second_theta += second_theta_per_frame;
        }
        
        // Increment our playback time
        m_playbackTimeInMs += (numSamples * msPerFrame);
    }
    
private:
    double m_playbackTimeInMs;
};


#if TARGET_OS_OSX
OSStatus synthAppIOProc (AudioDeviceID  inDevice, const AudioTimeStamp*  inNow, const AudioBufferList*   inInputData,
                         const AudioTimeStamp*  inInputTime, AudioBufferList*  outOutputData, const AudioTimeStamp* inOutputTime,
                         void* defptr)
{
    CAudioHAL * audioHal = (CAudioHAL *)defptr;
    
    UInt32 numSamples = audioHal->NumSamplesPerFrame();
    Float32 * out = (float *)outOutputData->mBuffers[0].mData;
    
    Float32 * stereoSamples = new Float32[numSamples * 2];
    assert(stereoSamples != 0);
    
    audioHal->GetFrames(stereoSamples, numSamples);
    
    memcpy(out, stereoSamples, numSamples * sizeof(Float32)*2);
    
    delete [] stereoSamples;
    
    return kAudioHardwareNoError;
}
#else
OSStatus RenderTone(
                    void *inRefCon,
                    AudioUnitRenderActionFlags     *ioActionFlags,
                    const AudioTimeStamp         *inTimeStamp,
                    UInt32                         inBusNumber,
                    UInt32                         inNumberFrames,
                    AudioBufferList             *ioData)

{
    CAudioHAL * audioHal = (CAudioHAL *)inRefCon;
    
    // This is a mono tone generator so we only need the first buffer
    const int channel = 0;
    Float32 *out = (Float32 *)ioData->mBuffers[channel].mData;
    
    Float32 * stereoSamples = new Float32[inNumberFrames * 2];
    
    audioHal->GetFrames(stereoSamples, inNumberFrames);
    
    // Mono only for now on iOS
    for (int i=0; i<inNumberFrames; i++)
        out[i] = (stereoSamples[i*2 + 0] + stereoSamples[i*2 + 1]) / 2.0;

    delete [] stereoSamples;
    
    return noErr;
}

void ToneInterruptionListener(void *inClientData, UInt32 inInterruptionState)
{
}
#endif // TARGET_OS_OSX

CAudioHAL::CAudioHAL()
: m_sampleRate(DEFAULT_SAMPLE_RATE),
m_audioUnit(NULL),
#if TARGET_OS_IPHONE
m_toneUnit(0),
#endif
m_fInitialized(false),
m_fPlaybackEnabled(false)
{
#if TARGET_OS_OSX
    this->CreateAudioDevice();
#endif
    
    // For testing
    m_audioUnit = new CToneUnit();
    
}

CAudioHAL::~CAudioHAL()
{
    this->DisablePlayback();
    delete m_audioUnit;
}

#if TARGET_OS_OSX
void CAudioHAL::CreateAudioDevice()
{
    OSStatus err = kAudioHardwareNoError;
    UInt32   count;
    m_device = kAudioDeviceUnknown;
    
    m_fInitialized = false;
    
    // get the default output device for the HAL
    count = sizeof(m_device);        // it is required to pass the size of the data to be returned
    err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice,  &count, (void *) &m_device);
    if (err != kAudioHardwareNoError) {
        fprintf(stderr, "get kAudioHardwarePropertyDefaultOutputDevice error %ld\n", err);
        return;
    }
    
    // get the buffersize that the default device uses for IO
    count = sizeof(m_deviceBufferSize);    // it is required to pass the size of the data to be returned
    err = AudioDeviceGetProperty(m_device, 0, false, kAudioDevicePropertyBufferSize, &count, &m_deviceBufferSize);
    if (err != kAudioHardwareNoError) {
        fprintf(stderr, "get kAudioDevicePropertyBufferSize error %ld\n", err);
        return;
    }
    fprintf(stderr, "deviceBufferSize = %ld\n", m_deviceBufferSize);
    
    // get a description of the data format used by the default device
    count = sizeof(m_deviceFormat);    // it is required to pass the size of the data to be returned
    err = AudioDeviceGetProperty(m_device, 0, false, kAudioDevicePropertyStreamFormat, &count, &m_deviceFormat);
    if (err != kAudioHardwareNoError) {
        fprintf(stderr, "get kAudioDevicePropertyStreamFormat error %ld\n", err);
        return;
    }
    if (m_deviceFormat.mFormatID != kAudioFormatLinearPCM) {
        fprintf(stderr, "mFormatID !=  kAudioFormatLinearPCM\n");
        return;
    }
    if (!(m_deviceFormat.mFormatFlags & kLinearPCMFormatFlagIsFloat)) {
        fprintf(stderr, "Sorry, currently only works with float format....\n");
        return;
    }
    
    m_fInitialized = true;
    
    
    m_sampleRate = m_deviceFormat.mSampleRate;
    fprintf(stderr, "mSampleRate = %g\n", m_deviceFormat.mSampleRate);
    fprintf(stderr, "mFormatFlags = %08lX\n", m_deviceFormat.mFormatFlags);
    fprintf(stderr, "mBytesPerPacket = %ld\n", m_deviceFormat.mBytesPerPacket);
    fprintf(stderr, "mFramesPerPacket = %ld\n", m_deviceFormat.mFramesPerPacket);
    fprintf(stderr, "mChannelsPerFrame = %ld\n", m_deviceFormat.mChannelsPerFrame);
    fprintf(stderr, "mBytesPerFrame = %ld\n", m_deviceFormat.mBytesPerFrame);
    fprintf(stderr, "mBitsPerChannel = %ld\n", m_deviceFormat.mBitsPerChannel);
}
#else
void CAudioHAL::CreateToneUnit()
{
    // Configure the search parameters to find the default playback output unit
    // (called the kAudioUnitSubType_RemoteIO on iOS but
    // kAudioUnitSubType_DefaultOutput on Mac OS X)
    AudioComponentDescription defaultOutputDescription;
    defaultOutputDescription.componentType = kAudioUnitType_Output;
    defaultOutputDescription.componentSubType = kAudioUnitSubType_RemoteIO;
    defaultOutputDescription.componentManufacturer = kAudioUnitManufacturer_Apple;
    defaultOutputDescription.componentFlags = 0;
    defaultOutputDescription.componentFlagsMask = 0;
    
    // Get the default playback output unit
    AudioComponent defaultOutput = AudioComponentFindNext(NULL, &defaultOutputDescription);
    assert(defaultOutput);
    
    // Create a new unit based on this that we'll use for output
    OSErr err = AudioComponentInstanceNew(defaultOutput, &m_toneUnit);
    //  NSAssert1(_synthdata.toneUnit, @"Error creating unit: %ld", err);
    
    // Set our tone rendering function on the unit
    AURenderCallbackStruct input;
    input.inputProc = RenderTone;
    input.inputProcRefCon = this;
    err = AudioUnitSetProperty(m_toneUnit,
                               kAudioUnitProperty_SetRenderCallback,
                               kAudioUnitScope_Input,
                               0,
                               &input,
                               sizeof(input));
    //  NSAssert1(err == noErr, @"Error setting callback: %ld", err);
    
    // Set the format to 32 bit, single channel, floating point, linear PCM
    const int four_bytes_per_float = 4;
    const int eight_bits_per_byte = 8;
    AudioStreamBasicDescription streamFormat;
    streamFormat.mSampleRate = m_sampleRate;
    streamFormat.mFormatID = kAudioFormatLinearPCM;
    streamFormat.mFormatFlags =
    kAudioFormatFlagsNativeFloatPacked | kAudioFormatFlagIsNonInterleaved;
    streamFormat.mBytesPerPacket = four_bytes_per_float;
    streamFormat.mFramesPerPacket = 1;
    streamFormat.mBytesPerFrame = four_bytes_per_float;
    streamFormat.mChannelsPerFrame = 1;
    streamFormat.mBitsPerChannel = four_bytes_per_float * eight_bits_per_byte;
    err = AudioUnitSetProperty (m_toneUnit,
                                kAudioUnitProperty_StreamFormat,
                                kAudioUnitScope_Input,
                                0,
                                &streamFormat,
                                sizeof(AudioStreamBasicDescription));
    // NSAssert1(err == noErr, @"Error setting stream format: %ld", err);
}
#endif

void CAudioHAL::EnablePlayback()
{
#if TARGET_OS_IPHONE
    this->CreateToneUnit();
    
    // Stop changing parameters on the unit
    OSErr err = AudioUnitInitialize(m_toneUnit);
    // NSAssert1(err == noErr, @"Error initializing unit: %ld", err);
    
    // Start playback
    err = AudioOutputUnitStart(m_toneUnit);
    // NSAssert1(err == noErr, @"Error starting unit: %ld", err);
    m_fInitialized = true;
#else
    OSStatus        err = kAudioHardwareNoError;
    
    if (!m_fInitialized) return;
    if (m_fPlaybackEnabled) return;
    
    // initialize phase and de-zipper filters.
    err = AudioDeviceAddIOProc(m_device, synthAppIOProc, (void *) this);    // setup our device with an IO proc
    if (err != kAudioHardwareNoError) return;
    
    err = AudioDeviceStart(m_device, synthAppIOProc);                // start playing sound through the device
    if (err != kAudioHardwareNoError) return;
    
#endif
    
    m_fPlaybackEnabled = true;
}

void CAudioHAL::DisablePlayback()
{
#if TARGET_OS_OSX
    OSStatus     err = kAudioHardwareNoError;
    
    if (!m_fInitialized) return;
    if (!m_fPlaybackEnabled) return;
    
    err = AudioDeviceStop(m_device, synthAppIOProc);                // stop playing sound through the device
    if (err != kAudioHardwareNoError) return;
    
    err = AudioDeviceRemoveIOProc(m_device, synthAppIOProc);            // remove the IO proc from the device
    if (err != kAudioHardwareNoError) return;
#elif TARGET_OS_IPHONE
    AudioOutputUnitStop(m_toneUnit);
    AudioUnitUninitialize(m_toneUnit);
    AudioComponentInstanceDispose(m_toneUnit);
    m_toneUnit = nil;
    m_fInitialized = false;
#else
#error Unsupported platform
#endif
    
    m_fPlaybackEnabled = false;
}

#if TARGET_OS_OSX
int CAudioHAL::NumSamplesPerFrame()
{
    return m_deviceBufferSize / m_deviceFormat.mBytesPerFrame;
}
#endif

void CAudioHAL::GetFrames(Float32 *out, UInt32 numFrames)
{
    if (m_audioUnit)
        m_audioUnit->GetFrames(out, numFrames, m_sampleRate);
    else
        memset(out, 0, numFrames*sizeof(Float32)*2);
}

void CAudioHAL::SetAudioUnit(IAudioHALUnit * audioUnit)
{
    // Can only call once
    assert(m_audioUnit == NULL);
    
    m_audioUnit = audioUnit;
}

double CAudioHAL::GetSampleRate()
{
    return m_sampleRate;
}
