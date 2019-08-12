//
//  GamePad.swift
//  SagaCross
//
//  Created by Marcin Małysz on 26/07/2019.
//  Copyright © 2019 Marcin Małysz. All rights reserved.
//

import UIKit

enum PadType {
    case Move
    case Aim
}

protocol GamePadDelegate: class {

    func gamePadDidFinish(_ gamePad: GamePad)
    func gamePadDidUpdate(_ gamePad: GamePad)
}

class GamePad: UIView {

    private let kAnimationTime = 0.25
    private let kMaximumLenght:CGFloat = 64.0

    private let background = UIImageView()
    private let pad = UIImageView()

    public var velocity = CGPoint()
    public var angle:CGFloat = 0.0

    weak var delegate: GamePadDelegate?

    override init(frame: CGRect) {

        super.init(frame: frame)
        commonInit()
    }

    private func commonInit() {

        let path = Bundle.main.resourcePath

        guard let resourcePath = path else {
            return
        }

        background.frame = CGRect(x: 0, y: 0, width: 128, height: 128)
        background.image = UIImage(contentsOfFile:resourcePath.appending("/resource/fr1.png"))

        addSubview(background)

        pad.frame = CGRect(x: 0, y: 0, width: 64, height: 64)
        addSubview(pad)

        background.alpha = 0.0
        pad.alpha = 0.0

        let tapRecognier = UITapGestureRecognizer(target: self, action: #selector(viewTapped))
        addGestureRecognizer(tapRecognier)

        let panRecognier = UIPanGestureRecognizer(target: self, action: #selector(viewPan(sender:)))
        addGestureRecognizer(panRecognier)
    }

    required init?(coder aDecoder: NSCoder) {

        super.init(coder: aDecoder)
        commonInit()
    }

    func set(type: PadType) {

        let path = Bundle.main.resourcePath

        guard let resourcePath = path else {
            return
        }

        pad.image = UIImage(contentsOfFile: type == .Move ? resourcePath.appending("/resource/blue_fr2.png") : resourcePath.appending("/resource/red_fr2.png"))
    }

    @objc func viewTapped() {

        delegate?.gamePadDidFinish(self)
    }

    @objc func viewPan(sender: UIPanGestureRecognizer) {

        var pos = background.center

        switch sender.state {

        case .began:
            background.center = sender.location(in: self)
            pad.center = background.center

            UIView.animate(withDuration: kAnimationTime) { [weak self] in

                self?.background.alpha = 1.0
                self?.pad.alpha = 1.0
            }

        case .changed:
            let trans = sender.translation(in: self)

            let rad = atan2(trans.x, trans.y)
            angle = ((rad * 180.0 / CGFloat.pi) + 360).truncatingRemainder(dividingBy: 360) 

            let a = sin(rad);
            let b = cos(rad);

            var len = sqrt(trans.x * trans.x + trans.y * trans.y);

            if (len > kMaximumLenght) {
                len = kMaximumLenght;
            }

            pos.x = a * len;
            pos.y = b * len;

            velocity = pos;

            delegate?.gamePadDidUpdate(self)

        case .ended:
            UIView.animate(withDuration: kAnimationTime) { [weak self] in

                self?.background.alpha = 0.0
                self?.pad.alpha = 0.0
            }

            velocity = CGPoint()
            //angle = 0

            delegate?.gamePadDidUpdate(self)
            delegate?.gamePadDidFinish(self)

        case .cancelled, .failed, .possible:
            velocity = CGPoint()
            angle = 0

        @unknown default:
            fatalError("unknown state")
        }

        pad.center = CGPoint(x: background.center.x + pos.x,
                             y: background.center.y + pos.y);
    }
}
