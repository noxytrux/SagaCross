package com.noclip.marcinmalysz.sagacross

import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.support.v4.content.ContextCompat
import android.Manifest
import android.content.pm.PackageManager
import android.graphics.Point
import android.os.Debug
import android.os.Environment
import android.support.constraint.ConstraintLayout
import android.support.v4.app.ActivityCompat
import android.util.Log
import android.view.*
import android.widget.FrameLayout
import android.widget.Toast
import java.io.*

class MainActivity : AppCompatActivity(), GamePadDelegate {

    val wrapper: SCGameWrapper = SCGameWrapper()
    val kWriteStorageCode = 1337
    var tapDetector: GestureDetector? = null

    var glView: SCGLView? = null
    var aimPad: GamePad? = null
    var movePad: GamePad? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        //fullscreen
        window.addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN)
        SCImmersiveMode.SetImmersiveMode(window)

        //copy assets
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED) {

            ActivityCompat.requestPermissions(
                this,
                arrayOf(Manifest.permission.WRITE_EXTERNAL_STORAGE),
                kWriteStorageCode
            )

            return
        }

        preapreForCopy()
    }

    override fun onPostResume() {
        super.onPostResume()

        SCImmersiveMode.SetImmersiveMode(window)
    }

    override fun onDestroy() {
        super.onDestroy()

        org.fmod.FMOD.close()
    }

    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<String>, grantResults: IntArray) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)

        if (requestCode == kWriteStorageCode) {

            if (grantResults.size > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {

                preapreForCopy()
            }
        }
    }

    private fun preapreForCopy() {

        Log.d("[STORAGE]", Environment.getExternalStorageDirectory().toString())

        val dir = File(Environment.getExternalStorageDirectory(), "SagaCross")

        if (!dir.exists()) {

            if (dir.mkdir()) {

                //Copy assets
                copyAssets()

            } else {

                val text = "Please enable external storage access for game"
                val duration = Toast.LENGTH_LONG

                val toast = Toast.makeText(applicationContext, text, duration)
                toast.setGravity(Gravity.TOP or Gravity.CENTER, 0, 0)
                toast.show()

                Log.d("[ERROR]", "COULD NOT CREATE DIRECTORY FOR ASSETS")
            }
        } else {

            Log.d("[INFO]", "DIRECTORY ALREADY EXIST")
        }

        val noMediaLocation = Environment.getExternalStorageDirectory().toString() + "/SagaCross/.nomedia"
        val noMediaFile = File(noMediaLocation)

        if (!noMediaFile.exists()) {

            try {

                noMediaFile.createNewFile()

            } catch (ignored: IOException) {

            }

        }

        org.fmod.FMOD.init(this)

        if (dir.exists()) {

            val display = this.windowManager.defaultDisplay
            val size = Point()
            display.getRealSize(size)
            val widthPixels = size.x
            val heightPixels = size.y

            glView = SCGLView(this, true, 24, 8)

            glView?.also {

                it.preserveEGLContextOnPause = true
                it.holder.setFixedSize(widthPixels, heightPixels)
                it.wrapper = wrapper

                val layout = this.findViewById<ConstraintLayout>(R.id.gameLayout)
                val params = FrameLayout.LayoutParams(widthPixels, heightPixels)

                layout.addView(it,0, params)

                tapDetector = GestureDetector(it.context, SCGestureTapListener())

                it.setOnTouchListener(object : View.OnTouchListener {

                    override fun onTouch(v: View, event: MotionEvent): Boolean {

                        tapDetector?.onTouchEvent(event)

                        return true
                    }
                })

//                aimPad = findViewById(R.id.aimPad)
//                movePad = findViewById(R.id.movePad)
//
//                aimPad?.padType = GamePad.PadType.PadAim
//                movePad?.padType = GamePad.PadType.PadMove
//
//                aimPad?.delegate = this
//                movePad?.delegate = this
            }
        }
    }

    @Throws(IOException::class)
    private fun copyFile(input: InputStream, output: OutputStream) {

        val buffer = ByteArray(1024)
        var read: Int = input.read(buffer)

        while (read != -1) {

            output.write(buffer, 0, read)
            read = input.read(buffer)
        }
    }

    private fun copyFileOrDir(path: String) {

        val assetManager = assets

        var files: Array<String>? = null

        try {

            files = assetManager.list(path)

        } catch (e: IOException) {

            Log.e("[ERROR]", "Failed to get asset file list.", e)
        }

        if (files == null) {
            return
        }

        val basePath = Environment.getExternalStorageDirectory().toString() + "/SagaCross"

        if (files.size == 0) {

            val filePath = basePath + "/" + path
            val outFile = File(filePath)

            try {

                assetManager.open(path).use { input ->

                    FileOutputStream(outFile).use { out ->

                        copyFile(input, out)
                    }
                }

            } catch (e: IOException) {

                Log.e("[ERROR]", "Failed to copy asset file: $filePath", e)
            }

        } else {

            val fullPath = basePath + "/" + path
            val dir = File(fullPath)

            if (!dir.exists()) {
                dir.mkdir()
            }

            for (filename in files) {

                Log.d("Files", "FileName:$filename")

                if (filename.startsWith("images") || filename.startsWith("webkit")) {
                    continue
                }

                if (path.isEmpty()) {

                    copyFileOrDir(filename)

                }
                else {

                    copyFileOrDir(path + "/" + filename)
                }
            }
        }
    }

    private fun copyAssets() {

        val text = "COPING ASSETS..."
        val duration = Toast.LENGTH_SHORT

        var toast = Toast.makeText(applicationContext, text, duration)
        toast.setGravity(Gravity.TOP or Gravity.CENTER, 0, 0)
        toast.show()

        copyFileOrDir("")

        val textFinish = "COPING ASSETS FINISHED."

        toast = Toast.makeText(applicationContext, textFinish, duration)
        toast.setGravity(Gravity.TOP or Gravity.CENTER, 0, 0)
        toast.show()

        Log.d("[INFO]", "COPING ASSETS FINISHED.")
    }

    private inner class SCGestureTapListener : GestureDetector.SimpleOnGestureListener() {

        override fun onSingleTapConfirmed(e: MotionEvent): Boolean {

            wrapper.handleTouch(e.rawX, e.rawY, true)

            return true
        }
    }

    override fun gamePadDidFinish(gamePad: GamePad) {

        if (gamePad == aimPad) {

            wrapper.fireBullet()
        }
    }

    override fun gamePadDidUpdate(gamePad: GamePad) {

        val x = movePad?.x ?: 0.0f
        val y = movePad?.y ?: 0.0f
        val angle = aimPad?.angle ?: 0.0f

        wrapper.handleMovement(x, y, angle)
    }

    companion object {

        init {

            System.loadLibrary("fmod")
            System.loadLibrary("fmodL")
            System.loadLibrary("native-lib")
        }
    }
}
