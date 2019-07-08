#pragma once

#define NUMBER_OF_COMPONENTS_PER_VERTEX 2
#define NUMBER_OF_INDICES 6

#ifdef __EMSCRIPTEN__

    #define GL_GLEXT_PROTOTYPES
    #include <GLES/gl.h>
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #define GLFW_INCLUDE_ES2
    #include <GLFW/glfw3.h>
    #include <emscripten/emscripten.h>

    #define glBindVertexArray glBindVertexArrayOES
    #define glGenVertexArrays glGenVertexArraysOES
    #define glDeleteVertexArrays glDeleteVertexArraysOES

#else

#ifdef _WIN32

	#define NOMINMAX
	#include <GL/glew.h>
	#include <GL/gl.h>
	#include <GLFW/glfw3.h>

#endif

#ifdef __APPLE__

    #define GL_SILENCE_DEPRECATION

    #if TARGET_OS_IPHONE

        #ifdef __OBJC__
            #import <Foundation/Foundation.h>
            #import <QuartzCore/QuartzCore.h>
            #import <OpenGLES/EAGL.h>
        #endif

        #import <OpenGLES/ES1/gl.h>
        #import <OpenGLES/ES1/glext.h>

        #import <OpenGLES/ES2/gl.h>
        #import <OpenGLES/ES2/glext.h>

        #define glBindVertexArray glBindVertexArrayOES
        #define glGenVertexArrays glGenVertexArraysOES
        #define glDeleteVertexArrays glDeleteVertexArraysOES

    #else

        #include <GL/glew.h>
        #include <GLFW/glfw3.h>

    #endif

#endif

#ifdef __ANDROID__

    #include <GLES/gl.h>
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
    #include <string.h>

    extern PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOES;
    extern PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOES;
    extern PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOES;

    extern void eglBuildVertexArray();

    #define glBindVertexArray glBindVertexArrayOES
    #define glGenVertexArrays glGenVertexArraysOES
    #define glDeleteVertexArrays glDeleteVertexArraysOES

#elif defined(__linux__)

    #ifdef _RPI_

        #include <GLES2/gl2.h>
        #include <GLES2/gl2ext.h>

        #include <EGL/egl.h>

        extern PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOES;
        extern PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOES;
        extern PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOES;

        extern void eglBuildVertexArray();

        #define glBindVertexArray glBindVertexArrayOES
        #define glGenVertexArrays glGenVertexArraysOES
        #define glDeleteVertexArrays glDeleteVertexArraysOES

        #define GLFW_INCLUDE_ES2

    #else

        #include <GL/glew.h>
        #include <GL/gl.h>

    #endif

        #include <GLFW/glfw3.h>

        #ifndef _RPI_
            #define GL_RED_EXT GL_RED
        #endif

    #endif

#endif

#if defined(__ANDROID__) || defined(TARGET_OS_IPHONE) || defined(__EMSCRIPTEN__)
#define MOBILE 1
#endif

#define OFFSET(st, m) \
((size_t) ( (char *)&((st *)(0))->m - (char *)0 ))

extern bool FLT_EQUAL(float a, float b);

#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif

#include <cassert>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <memory>
#include <thread>
#include <chrono>
#include <functional>
#include <map>
#include <set>
#include <cstdint>

#include "EnumOptionMacro.hpp"
#include "MathTypes.h"
#include "SCNonCopyable.hpp"

//since c++17
#if __cplusplus < 201703L
template<class T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi)
{
	return std::min(std::max(v, lo), hi);
}
#endif

extern std::vector<std::string> split(const std::string& s, char delimiter);

#include "json.hpp"
using json = nlohmann::json;


