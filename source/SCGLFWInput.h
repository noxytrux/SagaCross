#pragma once

#include "SCInputInterface.h"

namespace sc {

	class SCGLFWInput : public SCInputInteface
	{
	public:

		SCGLFWInput(void *context);
		virtual ~SCGLFWInput();
	
		virtual void update() override;

		virtual xVec2 getMousePosition() override;
		virtual xVec2 getDoubleClickPosition() override;

		virtual bool isLeftMouseButtonDown() override;
		virtual bool isRightMouseButtonDown() override;
		virtual bool isMiddleMouseButtonDown() override;
		virtual bool isDoubleClickDown() override;

	private:

		void sc_scroll_callback(GLFWwindow *win, double xoff, double yoff);
		void sc_mouse_callback(GLFWwindow *win, int button, int action, int mods);
		void sc_text_callback(GLFWwindow *win, unsigned int codepoint);
		void sc_key_callback(GLFWwindow *window);

		friend void scroll_callback(GLFWwindow *win, double xoff, double yoff);
		friend void mouse_callback(GLFWwindow *win, int button, int action, int mods);
		friend void text_callback(GLFWwindow *win, unsigned int codepoint);

	private:

		GLFWwindow *_window;
		double _lastButtonClick;
		xVec2 _doubleClickPos;
		bool _isDoubleClickDown;
	};
}


