//
//  AudioEngine.swift
//  Synthie
//
//  Created by Allen Ussher on 6/2/18.
//  Copyright © 2018 Ussher Press. All rights reserved.
//

import Foundation

class AudioEngine {
    var isPlaying: Bool = false
    let audioEngine = AudioEngineObjC()
    
    init() {
        // Set up audio callbacks
    }
    
    deinit {
        // Teardown
    }
    
    func playTone() {
        if !isPlaying {
            audioEngine.start()
            isPlaying = true
        }
    }
    
    func stopTone() {
        if isPlaying {
            audioEngine.stop()
            isPlaying = false
        }
    }
}
