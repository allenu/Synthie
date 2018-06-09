//
//  AudioEngineObjC.m
//  Synthie
//
//  Created by Allen Ussher on 6/9/18.
//  Copyright © 2018 Ussher Press. All rights reserved.
//

#import "AudioEngineObjC.h"
#import "AudioHAL.hpp"

@implementation AudioEngineObjC {
    CAudioHAL *_audioHAL;
}

- (id)init {
    self = [super init];
    
    if (self) {
        _audioHAL = new CAudioHAL();
    }
    
    return self;
}

- (void)deinit {
    delete _audioHAL;
}

- (void)start {
    _audioHAL->EnablePlayback();
}

- (void)stop {
    _audioHAL->DisablePlayback();
}

@end
