package com.noclip.marcinmalysz.sagacross

import android.content.Context
import android.util.AttributeSet
import android.widget.RelativeLayout
import android.widget.ImageView
import android.graphics.BitmapFactory
import android.graphics.Bitmap
import android.util.TypedValue
import java.io.IOException


class GamePad : RelativeLayout {

    enum class PadType {
        PadMove,
        PadAim
    }

    private var background: ImageView? = null
    private var pad: ImageView? = null

    var padType: PadType = PadType.PadMove
        get() = field
        set(value) { field = value; updatePadImage() }

    constructor(context: Context, attrs: AttributeSet) : super(context, attrs) {

        background = ImageView(this.context)
        pad = ImageView(this.context)

        addView(background)
        addView(pad)

        val bgSize = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 128.0f, resources.displayMetrics).toInt()

        background?.layoutParams?.width = bgSize
        background?.layoutParams?.height = bgSize
        background?.requestLayout()

        val padSize = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 64.0f, resources.displayMetrics).toInt()

        pad?.layoutParams?.width = padSize
        pad?.layoutParams?.height = padSize
        pad?.requestLayout()

        try {

            background?.setImageBitmap(getBitmapFromAsset("fr1.png"))
            pad?.setImageBitmap(getBitmapFromAsset("blue_fr2.png"))
        }
        catch (ignored: IOException) {

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
        val assetManager = context.assets
        val istr = assetManager.open(strName)
        return BitmapFactory.decodeStream(istr)
    }
}