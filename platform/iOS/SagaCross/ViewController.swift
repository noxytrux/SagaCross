//
//  ViewController.swift
//  SagaCross
//
//  Created by Marcin Małysz on 25/07/2019.
//  Copyright © 2019 Marcin Małysz. All rights reserved.
//

import UIKit
import GLKit

class ViewController: GLKViewController {

    let wrapper = SCGameWrapper()

    @IBOutlet weak var movePad: GamePad?
    @IBOutlet weak var aimPad: GamePad?

    override func viewDidLoad() {
        super.viewDidLoad()

        let view = self.view as? GLKView

        guard let glView = view else {
            return
        }

        glView.context = EAGLContext(api: EAGLRenderingAPI.openGLES2)!

        glView.drawableColorFormat     = GLKViewDrawableColorFormat.RGBA8888
        glView.drawableDepthFormat     = GLKViewDrawableDepthFormat.format24
        glView.drawableStencilFormat   = GLKViewDrawableStencilFormat.format8
        glView.drawableMultisample     = GLKViewDrawableMultisample.multisample4X

        self.preferredFramesPerSecond = 120; //120hz displays

        if (!EAGLContext.setCurrent(glView.context)) {

            print("[ERROR] Could not set active GLContext!")
        }


        //glView.contentScaleFactor = 1.0
        
        let scaleFactor = glView.contentScaleFactor
        let size = CGSize(width: self.view.frame.width * scaleFactor,
                          height: self.view.frame.height * scaleFactor)

        wrapper.initializeEngine(size)

        let recognizer = UITapGestureRecognizer(target: self, action: #selector(screenTouched(sender:)))
        recognizer.delegate = self
        
        self.view.addGestureRecognizer(recognizer)

        movePad?.set(type: .Move)
        aimPad?.set(type: .Aim)

        movePad?.delegate = self
        aimPad?.delegate = self
    }

    override func glkView(_ view: GLKView, drawIn rect: CGRect) {

        wrapper.renderGame()
    }

    @objc func screenTouched(sender: UITapGestureRecognizer) {

        var position = sender.location(in: self.view)

        position = CGPoint(x: position.x * self.view.contentScaleFactor,
                           y: position.y * self.view.contentScaleFactor)

        wrapper.handleTouch(position, selected: true)

        switch sender.state {

        case .possible, .began, .changed:
            ()

        case .ended:
            () //wrapper.handleTouch(position, selected: false)

        case .cancelled, .failed:
            wrapper.handleTouch(position, selected: false)

        @unknown default:
            fatalError("unknown state")
        }
    }
}

extension ViewController: GamePadDelegate {

    func gamePadDidFinish(_ gamePad: GamePad) {

        if gamePad == aimPad {

            wrapper.fireBullet()
        }
    }

    func gamePadDidUpdate(_ gamePad: GamePad) {

        guard let move = movePad, let aim = aimPad else {
            return
        }

        wrapper.handleMovement(move.velocity, angle: aim.angle)
    }
}

extension ViewController: UIGestureRecognizerDelegate {

    func gestureRecognizer(_ gestureRecognizer: UIGestureRecognizer, shouldRecognizeSimultaneouslyWith otherGestureRecognizer: UIGestureRecognizer) -> Bool {

        return true
    }
}
