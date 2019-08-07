package com.noclip.marcinmalysz.sagacross

import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.support.v4.content.ContextCompat
import android.view.WindowManager
import android.Manifest
import android.content.pm.PackageManager
import android.os.Environment
import android.support.v4.app.ActivityCompat
import android.util.Log
import android.view.GestureDetector
import android.view.Gravity
import android.view.MotionEvent
import android.widget.Toast
import java.io.*

class MainActivity : AppCompatActivity() {

    val wrapper: SCGameWrapper = SCGameWrapper()
    val kWriteStorageCode = 1337
    var tapDetector: GestureDetector? = null

    var glView: SCGLView? = null
    var aimPad: GamePad? = null
    var movePad: GamePad? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        aimPad = findViewById(R.id.aimPad)
        movePad = findViewById(R.id.movePad)

        //fullscreen
        window.addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN)
        SCImmersiveMode.SetImmersiveMode(window)

        tapDetector = GestureDetector(this, SCGestureTapListener())

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

    override fun onTouchEvent(event: MotionEvent?): Boolean {

        this.tapDetector?.onTouchEvent(event)

        return super.onTouchEvent(event)
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

        //TOOD: dodac plik .nomedia przy kopiowaniu
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

            //TODO: create here game engine
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

            Log.d("[INFO]", "SCREEN TAPPED.")

            return true
        }
    }

    companion object {

        init {

            System.loadLibrary("fmod")
            System.loadLibrary("fmodL")
            System.loadLibrary("native-lib")
        }
    }
}
