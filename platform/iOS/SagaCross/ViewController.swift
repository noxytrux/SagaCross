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

        print("Screen size: \(self.view.frame)")

        wrapper.initializeEngine(self.view.frame.size)
    }

    override func glkView(_ view: GLKView, drawIn rect: CGRect) {


    }
}

