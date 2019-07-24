#include "SCGLFWDisplay.h"

using namespace sc;

#define GL_CHK

template<typename T>
constexpr auto ARRAYSIZE(T p) { return sizeof(p)/sizeof(*p); }

static void error_callback(int e, const char *d) {

	std::cout << "Error " << e << ": " << d << std::endl;
}

#ifndef __EMSCRIPTEN__

#ifdef _WIN32
void __stdcall DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
#else
void DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
#endif
{
	const char* sourceStr = "UNDEFINED";
	switch (source)
	{
	case GL_DEBUG_SOURCE_API:				sourceStr = "API";					break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:		sourceStr = "WINDOW_SYSTEM";		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:	sourceStr = "SHADER_COMPILER";		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:		sourceStr = "THIRD_PARTY";			break;
	case GL_DEBUG_SOURCE_APPLICATION:		sourceStr = "APPLICATION";			break;
	case GL_DEBUG_SOURCE_OTHER:				sourceStr = "OTHER";				break;
	}

	const char *typeStr = "UNDEFINED";
	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:				typeStr = "ERROR";					break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:	typeStr = "DEPRECATED_BEHAVIOR";	break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:	typeStr = "UNDEFINED_BEHAVIOR";		break;
	case GL_DEBUG_TYPE_PORTABILITY:			typeStr = "PORTABILITY";			break;
	case GL_DEBUG_TYPE_PERFORMANCE:			typeStr = "PERFORMANCE";			break;
	case GL_DEBUG_TYPE_MARKER:				typeStr = "MARKER";					break;
	case GL_DEBUG_TYPE_PUSH_GROUP:			typeStr = "PUSH_GROUP";				break;
	case GL_DEBUG_TYPE_POP_GROUP:			typeStr = "POP_GROUP";				break;
	case GL_DEBUG_TYPE_OTHER:				typeStr = "OTHER";					break;
	}

	const char *severityStr = "UNDEFINED";
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:			severityStr = "HIGH";				break;
	case GL_DEBUG_SEVERITY_MEDIUM:			severityStr = "MEDIUM";				break;
	case GL_DEBUG_SEVERITY_LOW:				severityStr = "LOW";				break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:	severityStr = "NOTIF";				break;
	}

	printf("OpenGL: %s [source=%s type=%s severity=%s id=%u]\n", message, sourceStr, typeStr, severityStr, id);
}

void EnableOpenGLErrorCallback()
{
	glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	if (glfwExtensionSupported("GL_ARB_debug_output"))
	{
		glDebugMessageCallback(DebugCallback, nullptr);
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

		const uint32_t idsUndefined[] =
		{
			131222,	// The current GL state uses a sampler (0) that has depth comparisons disabled, with a texture object (0) with a non-depth format, by a shader that samples it with a shadow sampler. Using this state to sample would result in undefined behavior
		};

		const uint32_t idsPerformance[] =
		{
			131218, // program is being recompiled based on GL state
			131186,	// Buffer object is being copied/moved from VIDEO memory to HOST memory
		};

		// Disable some annoying warning messages
		glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, GL_DONT_CARE, ARRAYSIZE(idsUndefined), idsUndefined, false);
		glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_PERFORMANCE, GL_DONT_CARE, ARRAYSIZE(idsPerformance), idsPerformance, false);

		// Disable notification messages
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
	}
}

void DisableOpenGLErrorCallback()
{
	if (glfwExtensionSupported("GL_ARB_debug_output"))
	{
		glDisable(GL_DEBUG_OUTPUT);
		glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(nullptr, nullptr);
	}
}

#endif

SCGLFWDisplay::SCGLFWDisplay(const uint32_t width, const uint32_t height, const bool fullscreen) 
	: SCDisplay(width, height, fullscreen)
	, _win(nullptr)
{
}

SCGLFWDisplay::~SCGLFWDisplay() noexcept
{ 
	#ifndef __EMSCRIPTEN__
	DisableOpenGLErrorCallback();
	#endif

	glfwDestroyWindow(_win);
	glfwTerminate();
}

void SCGLFWDisplay::makeWindow()
{
	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {

		std::cout << "[GFLW] failed to init!" << std::endl;
		exit(1);
	}

#ifdef _RPI_
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#else
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif // _RPI_

	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#if defined(_DEBUG) || defined(DEBUG)

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	glfwWindowHint(GLFW_CONTEXT_NO_ERROR, GLFW_FALSE);

#endif

	_win = glfwCreateWindow(_size.width, _size.height, "SagaCross", _fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);

	glfwMakeContextCurrent(_win);

#ifndef __EMSCRIPTEN__
#ifndef _RPI_
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {

		std::cout << "Failed to setup GLEW" << std::endl;
		exit(1);
	}
#else
	eglBuildVertexArray();
#endif // _RPI_
#endif

#if defined(_DEBUG) || defined(DEBUG)

    if (!glfwExtensionSupported("GL_ARB_debug_output")) {
        std::cout << "[ERROR] Could not enable OpenGL debugging" << std::endl;
    }
    
    #ifndef __EMSCRIPTEN__
	EnableOpenGLErrorCallback();
	#endif

#endif

}

const bool SCGLFWDisplay::shouldClose() const {

	return glfwWindowShouldClose(_win);
}

void SCGLFWDisplay::resizeWindow(const SCScreenSize & size)
{
	if (!_win) {
		std::cout << "Window not created!" << std::endl;
		return;
	}

	glfwSetWindowSize(_win, size.width, size.height);
	_size = size;
}

void SCGLFWDisplay::resizeWindow(uint32_t width, uint32_t height)
{
	resizeWindow({ width, height });
}

const std::vector<SCScreenSize> SCGLFWDisplay::getResolutions()
{
	int count;
	const GLFWvidmode* modes = glfwGetVideoModes(glfwGetPrimaryMonitor(), &count);

	std::set<SCScreenSize> res;

	for (int i = 0; i < count; ++i) {
	
		const GLFWvidmode &mode = modes[i];

		if (mode.width < 1024) {
			continue;
		}

		res.insert({ static_cast<uint32_t>(mode.width), static_cast<uint32_t>(mode.height) });
	}

	std::vector<SCScreenSize> sizes(res.begin(), res.end());

	return sizes;
}

void SCGLFWDisplay::setFullScreen(bool fullscreen)
{
	SCDisplay::setFullScreen(fullscreen);

	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	if (fullscreen) {
	
		glfwSetWindowMonitor(_win, glfwGetPrimaryMonitor(), 0, 0, _size.width, _size.height, 0);
	}
	else {
	
		glfwSetWindowMonitor(_win, nullptr, (mode->width - _size.width) * 0.5, (mode->height - _size.height) * 0.5, _size.width, _size.height, 0);
	}
}

void SCGLFWDisplay::setVsync(const bool vsync)
{
	glfwSwapInterval(vsync);
}
