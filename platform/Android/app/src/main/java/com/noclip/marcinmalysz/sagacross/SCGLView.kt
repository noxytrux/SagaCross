package com.noclip.marcinmalysz.sagacross

import android.content.Context
import android.graphics.PixelFormat
import android.opengl.GLSurfaceView
import android.os.Environment
import android.util.AttributeSet
import android.view.SurfaceHolder

import javax.microedition.khronos.egl.EGL10
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.egl.EGLContext
import javax.microedition.khronos.egl.EGLDisplay
import javax.microedition.khronos.opengles.GL10

class SCGLView : GLSurfaceView {

    var wrapper: SCGameWrapper? = null
        get() = field
        set(value) { field = value; renderer?.wrapper = value }

    private var renderer: Renderer? = null

    constructor(context: Context, attrs: AttributeSet) : super(context, attrs) {

        init(false, 24, 8)
    }

    constructor(context: Context) : super(context) {

        init(false, 0, 0)
    }

    constructor(context: Context, translucent: Boolean, depth: Int, stencil: Int) : super(context) {

        init(translucent, depth, stencil)
    }

    private fun init(translucent: Boolean, depth: Int, stencil: Int) {

        isClickable = false
        isFocusable = false

        if (translucent) {
            this.holder.setFormat(PixelFormat.TRANSLUCENT)
        }

        setEGLContextFactory(ContextFactory())

        setEGLConfigChooser(

            if (translucent)
                ConfigChooser(8, 8, 8, 8, depth, stencil)
            else
                ConfigChooser(5, 6, 5, 0, depth, stencil)
        )

        preserveEGLContextOnPause = true
    }

    fun setupRenderer() {

        renderer = Renderer()

        setRenderer(renderer)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        super.surfaceDestroyed(holder)

        renderer?.loaded = false
    }

    private class ContextFactory : EGLContextFactory {

        override fun createContext(egl: EGL10, display: EGLDisplay, eglConfig: EGLConfig): EGLContext {

            val attrib_list = intArrayOf(EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE)
            val context = egl.eglCreateContext(display, eglConfig, EGL10.EGL_NO_CONTEXT, attrib_list)

            return context
        }

        override fun destroyContext(egl: EGL10, display: EGLDisplay, context: EGLContext) {
            egl.eglDestroyContext(display, context)
        }

        companion object {

            private val EGL_CONTEXT_CLIENT_VERSION = 0x3098
        }
    }

    private class ConfigChooser internal constructor(
        internal var mRedSize: Int,
        internal var mGreenSize: Int,
        internal var mBlueSize: Int,
        internal var mAlphaSize: Int,
        internal var mDepthSize: Int,
        internal var mStencilSize: Int
    ) : EGLConfigChooser {

        private val mValue = IntArray(1)

        override fun chooseConfig(egl: EGL10, display: EGLDisplay): EGLConfig? {

            val num_config = IntArray(1)
            egl.eglChooseConfig(display, s_configAttribs2, null, 0, num_config)

            val numConfigs = num_config[0]

            if (numConfigs <= 0) {

                throw IllegalArgumentException("No configs match configSpec")
            }

            val configs = arrayOfNulls<EGLConfig>(numConfigs)

            egl.eglChooseConfig(display, s_configAttribs2, configs, numConfigs, num_config)

            return chooseConfig(egl, display, configs)
        }

        internal fun chooseConfig(
            egl: EGL10, display: EGLDisplay,
            configs: Array<EGLConfig?>
        ): EGLConfig? {

            for (config in configs) {

                val conf = config!!

                val d = findConfigAttrib(
                    egl, display, conf,
                    EGL10.EGL_DEPTH_SIZE, 0
                )
                val s = findConfigAttrib(
                    egl, display, conf,
                    EGL10.EGL_STENCIL_SIZE, 0
                )

                // We need at least mDepthSize and mStencilSize bits
                if (d < mDepthSize || s < mStencilSize)
                    continue

                // We want an *exact* match for red/green/blue/alpha
                val r = findConfigAttrib(
                    egl, display, conf,
                    EGL10.EGL_RED_SIZE, 0
                )
                val g = findConfigAttrib(
                    egl, display, conf,
                    EGL10.EGL_GREEN_SIZE, 0
                )
                val b = findConfigAttrib(
                    egl, display, conf,
                    EGL10.EGL_BLUE_SIZE, 0
                )
                val a = findConfigAttrib(
                    egl, display, conf,
                    EGL10.EGL_ALPHA_SIZE, 0
                )

                if (r == mRedSize && g == mGreenSize && b == mBlueSize && a == mAlphaSize)
                    return config
            }

            return null
        }

        private fun findConfigAttrib(
            egl: EGL10, display: EGLDisplay,
            config: EGLConfig, attribute: Int, defaultValue: Int
        ) : Int {

            return if (egl.eglGetConfigAttrib(display, config, attribute, mValue)) {
                mValue[0]
            } else defaultValue
        }

        companion object {

            private val EGL_OPENGL_ES2_BIT = 4
            private val s_configAttribs2 = intArrayOf(
                EGL10.EGL_RED_SIZE,
                4,
                EGL10.EGL_GREEN_SIZE,
                4,
                EGL10.EGL_BLUE_SIZE,
                4,
                EGL10.EGL_RENDERABLE_TYPE,
                EGL_OPENGL_ES2_BIT,
                EGL10.EGL_NONE
            )
        }
    }

    private class Renderer : GLSurfaceView.Renderer {

        internal var loaded = false
        internal var wrapper: SCGameWrapper? = null

        override fun onDrawFrame(gl: GL10) {

            if (!loaded) return

            wrapper?.renderGame()
        }

        override fun onSurfaceChanged(gl: GL10, width: Int, height: Int) {

            val rootDir = Environment.getExternalStorageDirectory().toString() + "/SagaCross/"

            wrapper?.initializeEngine(width, height, rootDir)
            loaded = true
        }

        override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
            // Do nothing.
        }
    }
}