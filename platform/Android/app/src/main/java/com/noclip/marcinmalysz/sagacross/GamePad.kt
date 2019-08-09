package com.noclip.marcinmalysz.sagacross

import android.content.Context
import android.util.AttributeSet
import android.widget.RelativeLayout
import android.widget.ImageView
import android.graphics.BitmapFactory
import android.graphics.Bitmap
import android.graphics.PointF
import android.util.TypedValue
import java.io.IOException
import android.view.MotionEvent
import android.view.View
import android.view.animation.LinearInterpolator
import kotlin.math.*

class GamePad : RelativeLayout {

    enum class PadType {
        PadMove,
        PadAim
    }

    private var background: ImageView? = null
    private var pad: ImageView? = null

    var angle = 0.0f
    var velocity = PointF(0.0f, 0.0f)

    var padType: PadType = PadType.PadMove
        get() = field
        set(value) { field = value; updatePadImage() }

    private var sx = 0.0f
    private var sy = 0.0f

    constructor(context: Context, attrs: AttributeSet) : super(context, attrs) {

        background = ImageView(this.context)
        pad = ImageView(this.context)

        addView(background)
        addView(pad)

        val bgSize = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 128.0f, resources.displayMetrics).toInt()

        background?.also {

            it.layoutParams.width = bgSize
            it.layoutParams.height = bgSize
            it.requestLayout()
            it.alpha = 0.0f
        }

        val padSize = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 64.0f, resources.displayMetrics).toInt()

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

        setOnTouchListener(object : View.OnTouchListener {

            private val kMaximumLenght = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 64.0f, resources.displayMetrics)

            override fun onTouch(v: View, event: MotionEvent): Boolean {

                when (event.action) {

                    MotionEvent.ACTION_DOWN -> {

                        sx = event.x
                        sy = event.y

                        background?.also {

                            it.x = event.x - it.layoutParams.width * 0.5f
                            it.y = event.y - it.layoutParams.height * 0.5f

                            it.animate().apply {
                                interpolator = LinearInterpolator()
                                duration = 250
                                alpha(1.0f)
                                startDelay = 0
                                start()
                            }
                        }

                        pad?.also {

                            it.x = event.x - it.layoutParams.width * 0.5f
                            it.y = event.y - it.layoutParams.height * 0.5f

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

                            val tx = event.x - sx
                            val ty = event.y - sy

                            val rad = atan2(tx, ty)
                            angle = ((rad * 180.0f / PI.toFloat()) + 360.0f).rem(360)

                            val a = sin(rad)
                            val b = cos(rad)

                            var len = sqrt(tx * tx + ty * ty)
                                len = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, len, resources.displayMetrics)

                            if (len > kMaximumLenght) {

                                len = kMaximumLenght
                            }

                            velocity = PointF(a * len, b * len)

                            background?.also { bg ->

                                val cx = bg.x + bg.layoutParams.width * 0.5f
                                val cy = bg.y + bg.layoutParams.height * 0.5f

                                it.x = (cx + velocity.x) - it.layoutParams.width * 0.5f
                                it.y = (cy + velocity.y) - it.layoutParams.height * 0.5f
                            }
                        }
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
                    }
                }

                return true
            }
        })
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
        val assetManager = context.assets
        val istr = assetManager.open(strName)
        return BitmapFactory.decodeStream(istr)
    }
}