package com.noclip.marcinmalysz.sagacross

import android.content.Context
import android.util.AttributeSet
import android.widget.RelativeLayout
import android.widget.ImageView
import android.graphics.BitmapFactory
import android.graphics.Bitmap
import android.graphics.PointF
import android.support.constraint.ConstraintLayout
import android.support.v4.view.ViewCompat
import android.util.TypedValue
import java.io.IOException
import android.view.MotionEvent
import android.view.View
import android.view.animation.LinearInterpolator
import android.widget.FrameLayout
import kotlin.math.*

interface GamePadDelegate {

    fun gamePadDidFinish(gamePad: GamePad)
    fun gamePadDidUpdate(gamePad: GamePad)
}

class GamePad {

    enum class PadType {
        PadMove,
        PadAim
    }

    private var context:Context? = null
    private var layout:ConstraintLayout? = null

    //This is Kotlin you do not need to use WeakReference Retain cycle does not affect this just remove it later
    var delegate: GamePadDelegate? = null

    private var background: ImageView? = null
    private var pad: ImageView? = null

    var angle = 0.0f
    var velocity = PointF(0.0f, 0.0f)

    var padType: PadType = PadType.PadMove
        get() = field
        set(value) { field = value; updatePadImage() }

    private var sx = 0.0f
    private var sy = 0.0f

    //is this really bad idea to use it or
    fun finalize() {

        delegate = null
    }

    constructor(context: Context, layout: ConstraintLayout){

        this.context = context
        this.layout= layout

        commonInit()
    }

    private fun commonInit() {

        background = ImageView(this.context)
        pad = ImageView(this.context)

        layout?.addView(background)
        layout?.addView(pad)

        val metrics = context?.resources?.displayMetrics

        val bgSize = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 128.0f, metrics).toInt()

        background?.also {

            it.layoutParams.width = bgSize
            it.layoutParams.height = bgSize
            it.requestLayout()
            it.alpha = 0.0f
        }

        val padSize = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 64.0f, metrics).toInt()

        pad?.also {

            it.layoutParams.width = padSize
            it.layoutParams.height = padSize
            it.requestLayout()
            it.alpha = 0.0f
        }

        try {

            background?.setImageBitmap(getBitmapFromAsset("fr1.png"))
            pad?.setImageBitmap(getBitmapFromAsset("blue_fr2.png"))
        }
        catch (ignored: IOException) {

        }
    }

    fun onTouchEvent(event: MotionEvent) {

        val metrics = context?.resources?.displayMetrics
        val kMaximumLength = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 64.0f, metrics)

        val index:Int = event.actionIndex
        val size = layout!!.layoutParams.width * 0.5f

        if (padType == PadType.PadMove && event.getX(index) > size) {
            return
        }

        if (padType == PadType.PadAim && event.getX(index) < size) {
            return
        }

        when (event.action) {

            MotionEvent.ACTION_DOWN -> {

                sx = event.getX(index)
                sy = event.getY(index)

                background?.also {

                    it.x = event.getX(index) - it.layoutParams.width * 0.5f
                    it.y = event.getY(index) - it.layoutParams.height * 0.5f

                    it.animate().apply {
                        interpolator = LinearInterpolator()
                        duration = 250
                        alpha(1.0f)
                        startDelay = 0
                        start()
                    }
                }

                pad?.also {

                    it.x = event.getX(index) - it.layoutParams.width * 0.5f
                    it.y = event.getY(index) - it.layoutParams.height * 0.5f

                    it.animate().apply {
                        interpolator = LinearInterpolator()
                        duration = 250
                        alpha(1.0f)
                        startDelay = 0
                        start()
                    }
                }
            }

            MotionEvent.ACTION_MOVE -> {

                pad?.also {

                    val tx = event.getX(index) - sx
                    val ty = event.getY(index) - sy

                    val rad = atan2(tx, ty)
                    angle = ((rad * 180.0f / PI.toFloat()) + 360.0f).rem(360)

                    val a = sin(rad)
                    val b = cos(rad)

                    var len = sqrt(tx * tx + ty * ty)
                    len = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, len, metrics)

                    if (len > kMaximumLength) {

                        len = kMaximumLength
                    }

                    velocity = PointF(a * len, b * len)

                    background?.also { bg ->

                        val cx = bg.x + bg.layoutParams.width * 0.5f
                        val cy = bg.y + bg.layoutParams.height * 0.5f

                        it.x = (cx + velocity.x) - it.layoutParams.width * 0.5f
                        it.y = (cy + velocity.y) - it.layoutParams.height * 0.5f
                    }
                }

                delegate?.gamePadDidUpdate(this@GamePad)
            }

            MotionEvent.ACTION_UP -> {

                background?.also {

                    it.animate().apply {
                        interpolator = LinearInterpolator()
                        duration = 250
                        alpha(0.0f)
                        startDelay = 0
                        start()
                    }
                }

                pad?.also {

                    it.animate().apply {
                        interpolator = LinearInterpolator()
                        duration = 250
                        alpha(0.0f)
                        startDelay = 0
                        start()
                    }
                }

                angle = 0.0f
                velocity = PointF(0.0f, 0.0f)

                delegate?.gamePadDidUpdate(this@GamePad)
                delegate?.gamePadDidFinish(this@GamePad)
            }
        }

    }

    private fun updatePadImage() {

        try {

            when(padType) {

                PadType.PadAim -> pad?.setImageBitmap(getBitmapFromAsset("red_fr2.png"))
                PadType.PadMove -> pad?.setImageBitmap(getBitmapFromAsset("blue_fr2.png"))
            }
        }
        catch (ignored: IOException) {

        }
    }

    @Throws(IOException::class)
    private fun getBitmapFromAsset(strName: String): Bitmap {
        val assetManager = context?.assets
        val istr = assetManager?.open(strName)
        return BitmapFactory.decodeStream(istr)
    }
}