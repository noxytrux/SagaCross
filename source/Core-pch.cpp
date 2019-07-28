#include "Core-pch.hpp"

#if defined(__ANDROID__) || defined(_RPI_)

PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOES = nullptr;
PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOES = nullptr;
PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOES = nullptr;
PFNGLMAPBUFFEROESPROC glMapBufferOES = nullptr;
PFNGLUNMAPBUFFEROESPROC glUnmapBufferOES = nullptr;

void eglBuildVertexArray() {

    glBindVertexArrayOES = (PFNGLBINDVERTEXARRAYOESPROC)eglGetProcAddress("glBindVertexArrayOES");
    glGenVertexArraysOES = (PFNGLGENVERTEXARRAYSOESPROC)eglGetProcAddress("glGenVertexArraysOES");
    glDeleteVertexArraysOES = (PFNGLDELETEVERTEXARRAYSOESPROC)eglGetProcAddress("glDeleteVertexArraysOES");
    glMapBufferOES = (PFNGLMAPBUFFEROESPROC)eglGetProcAddress("glMapBufferOES");
    glUnmapBufferOES = (PFNGLUNMAPBUFFEROESPROC)eglGetProcAddress("glUnmapBufferOES");
}

#endif

bool FLT_EQUAL(float a, float b) {

    return fabs(a - b) < FLT_EPSILON;
}

