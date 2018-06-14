//
//  ViewController.swift
//  PocketSynthie
//
//  Created by Allen Ussher on 6/9/18.
//  Copyright © 2018 Ussher Press. All rights reserved.
//

import UIKit

class ViewController: UIViewController {
    let audioEngine = AudioEngine()
    @IBOutlet weak var playButton: UIButton!

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    @IBAction func didTapPlay(sender: UIButton?) {
        if !audioEngine.isPlaying {
            audioEngine.playTone()
            playButton.setTitle("Stop", for: .normal)
        } else {
            audioEngine.stopTone()
            playButton.setTitle("Play", for: .normal)
        }
    }
}
