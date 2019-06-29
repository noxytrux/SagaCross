#include "SCGLFWDisplay.h"

using namespace sc;

static void error_callback(int e, const char *d) {

	std::cout << "Error " << e << ": " << d << std::endl;
}

SCGLFWDisplay::SCGLFWDisplay(const uint32_t width, const uint32_t height, const bool fullscreen) 
	: SCDisplay(width, height, fullscreen)
	, _win(nullptr)
{
}

SCGLFWDisplay::~SCGLFWDisplay()
{ 
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
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif // _RPI_

	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	_win = glfwCreateWindow(_size.width, _size.height, "SagaCross", _fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);

	glfwMakeContextCurrent(_win);

#ifndef _RPI_
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {

		std::cout << "Failed to setup GLEW" << std::endl;
		exit(1);
	}
#else
	eglBuildVertexArray();
#endif // _RPI_

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
