package com.noclip.marcinmalysz.sagacross

class SCGameWrapper {

    external fun initializeEngine(width:Int, height:Int, path: String)
    external fun renderGame()
    external fun handleTouch(x:Float, y: Float, selected: Boolean)
    external fun handleMovement(dx: Float, dy: Float, angle: Float)
    external fun fireBullet()
    external fun dropMine()
    external fun renderingGame() : Boolean
}