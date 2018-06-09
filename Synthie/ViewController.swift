//
//  ViewController.swift
//  Synthie
//
//  Created by Allen Ussher on 6/2/18.
//  Copyright © 2018 Ussher Press. All rights reserved.
//

import Cocoa

class ViewController: NSViewController {
    let audioEngine = AudioEngine()
    @IBOutlet weak var playButton: NSButton!

    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }

    @IBAction func didTapPlay(sender: AnyObject?) {
        if !audioEngine.isPlaying {
            audioEngine.playTone()
            playButton.title = "Stop"
        } else {
            audioEngine.stopTone()
            playButton.title = "Play"
        }
    }
}
