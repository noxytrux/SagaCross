#include "Core-pch.hpp"

#if defined(__ANDROID__) || defined(_RPI_)

PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOES = nullptr;
PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOES = nullptr;
PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOES = nullptr;

void eglBuildVertexArray() {

    glBindVertexArrayOES = (PFNGLBINDVERTEXARRAYOESPROC)eglGetProcAddress("glBindVertexArrayOES");
    glGenVertexArraysOES = (PFNGLGENVERTEXARRAYSOESPROC)eglGetProcAddress("glGenVertexArraysOES");
    glDeleteVertexArraysOES = (PFNGLDELETEVERTEXARRAYSOESPROC)eglGetProcAddress("glDeleteVertexArraysOES");
}

#endif

bool FLT_EQUAL(float a, float b) {

    return fabs(a - b) < FLT_EPSILON;
}

