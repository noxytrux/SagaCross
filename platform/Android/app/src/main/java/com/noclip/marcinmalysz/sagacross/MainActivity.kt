package com.noclip.marcinmalysz.sagacross

import android.support.v7.app.AppCompatActivity
import android.os.Bundle

class MainActivity : AppCompatActivity() {

    val wrapper: SCGameWrapper = SCGameWrapper()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)


    }

    companion object {

        init {

            System.loadLibrary("fmod")
            System.loadLibrary("fmodL")
            System.loadLibrary("native-lib")
        }
    }
}
