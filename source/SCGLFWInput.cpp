#include "SCGLFWInput.h"

namespace sc {

	void scroll_callback(GLFWwindow *win, double xoff, double yoff);
	void mouse_callback(GLFWwindow *win, int button, int action, int mods);
	void text_callback(GLFWwindow *win, unsigned int codepoint);
}

using namespace sc;

constexpr float NK_GLFW_DOUBLE_CLICK_LO  = 0.02;
constexpr float NK_GLFW_DOUBLE_CLICK_HI = 0.2;

void sc::scroll_callback(GLFWwindow *win, double xoff, double yoff)
{
	static_cast<SCGLFWInput*>(glfwGetWindowUserPointer(win))->sc_scroll_callback(win, xoff, yoff);
}

void sc::mouse_callback(GLFWwindow *win, int button, int action, int mods)
{
	static_cast<SCGLFWInput*>(glfwGetWindowUserPointer(win))->sc_mouse_callback(win, button, action, mods);
}

void sc::text_callback(GLFWwindow *win, unsigned int codepoint)
{
	static_cast<SCGLFWInput*>(glfwGetWindowUserPointer(win))->sc_text_callback(win, codepoint);
}

SCGLFWInput::SCGLFWInput(void *context) 
	: SCInputInteface(context)
	, _window(static_cast<GLFWwindow*>(context))
	, _lastButtonClick(0)
	, _doubleClickPos(xVec2())
	, _isDoubleClickDown(false)
{
	glfwSetWindowUserPointer(_window, this);
	glfwSetScrollCallback(_window, scroll_callback);
	glfwSetMouseButtonCallback(_window, mouse_callback);
	glfwSetCharCallback(_window, text_callback);
}

SCGLFWInput::~SCGLFWInput()
{
	glfwSetWindowUserPointer(_window, nullptr);
	_window = nullptr;
}

void SCGLFWInput::update()
{
	glfwPollEvents();
	sc_key_callback(_window);
}

xVec2 SCGLFWInput::getMousePosition()
{
	double x, y;
	glfwGetCursorPos(_window, &x, &y);
	return xVec2(x, y);
}

xVec2 SCGLFWInput::getDoubleClickPosition()
{
	return _doubleClickPos;
}

bool SCGLFWInput::isLeftMouseButtonDown()
{
	return glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
}

bool SCGLFWInput::isRightMouseButtonDown()
{
	return glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
}

bool SCGLFWInput::isMiddleMouseButtonDown()
{
	return glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
}

bool SCGLFWInput::isDoubleClickDown()
{
	return _isDoubleClickDown;
}

void SCGLFWInput::sc_scroll_callback(GLFWwindow *win, double xoff, double yoff)
{
#ifndef __EMSCRIPTEN__
	if (scrollCallback) {
	
		scrollCallback(xVec2(xoff, yoff));
	}
#endif
}

void SCGLFWInput::sc_mouse_callback(GLFWwindow *win, int button, int action, int mods)
{
	double x, y;

	glfwGetCursorPos(win, &x, &y);

	if (mouseCallback) {
	
		mouseCallback(button, action, xVec2(x, y));
	}

	if (button != GLFW_MOUSE_BUTTON_LEFT) {

		return;
	}

	if (action == GLFW_PRESS) {

		double dt = glfwGetTime() - _lastButtonClick;

		if (dt > NK_GLFW_DOUBLE_CLICK_LO && dt < NK_GLFW_DOUBLE_CLICK_HI) {

			_isDoubleClickDown = true;
			_doubleClickPos = xVec2(x, y);
		}

		_lastButtonClick = glfwGetTime();

	}
	else {

		_isDoubleClickDown = false;
	}
}

void SCGLFWInput::sc_text_callback(GLFWwindow *win, unsigned int codepoint)
{
#ifndef __EMSCRIPTEN__
	if (textCallback) {
	
		textCallback(codepoint);
	}
#endif
}

void SCGLFWInput::sc_key_callback(GLFWwindow *window)
{
	xVec2 direction = xVec2(0, 0);
	float angle = 180;
	float moveSpeed = 50;

	if (glfwGetKey(window, GLFW_KEY_A) || glfwGetKey(window, GLFW_KEY_LEFT)) {

		direction.x = -moveSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_D) || glfwGetKey(window, GLFW_KEY_RIGHT)) {

		direction.x = moveSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_W) || glfwGetKey(window, GLFW_KEY_UP)) {

		direction.y = -moveSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_S) || glfwGetKey(window, GLFW_KEY_DOWN)) {

		direction.y = moveSpeed;
	}

	double x, y;
	glfwGetCursorPos(window, &x, &y);

#ifdef __EMSCRIPTEN__

	float rad = atan2(x - (1280 * 0.25), y - (720 * 0.25));

#else
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	float rad = atan2(x - (mode->width * 0.25), y - (mode->height * 0.25));
#endif

	angle = (int)((rad * 180.0 / M_PI) + 360.0) % 360;

	if (movementCallback) {

		movementCallback(direction, angle);
	}
}
