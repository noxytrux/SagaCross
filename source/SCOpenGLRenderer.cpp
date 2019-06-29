#include "SCOpenGLRenderer.h"
#include "SCGLFWDisplay.h"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#define NK_IMPLEMENTATION
#include "nuklear_glfw_gl3.h"

using namespace sc;

SCOpenGLRenderable::SCOpenGLRenderable(const std::shared_ptr<SCDisplay> &display, const std::shared_ptr<SCSettings> &settings) 
	: SCRendererInterface(display, settings)
{
	const auto &size = display->getScreenSize();

	//TODO: this may require refresh when screen size change
	_ctx = nk_glfw3_init(size.width, size.height);
}

SCOpenGLRenderable::~SCOpenGLRenderable()
{
	nk_glfw3_shutdown();
}

void SCOpenGLRenderable::loadFonts(const std::string &path)
{
	auto orbitronPath = path + "orbitron-light.ttf";

	nk_glfw3_font_stash_begin(&_atlas);
	_orbitron30 = nk_font_atlas_add_from_file(_atlas, orbitronPath.c_str(), 50, 0);
	_orbitron15 = nk_font_atlas_add_from_file(_atlas, orbitronPath.c_str(), 25, 0);
	nk_glfw3_font_stash_end();

	nk_style_set_font(_ctx, &_orbitron30->handle);

	_fontlist.push_back(_orbitron30);
	_fontlist.push_back(_orbitron15);
}

void SCOpenGLRenderable::updateViewPort(const SCScreenSize &screenSize)
{
	nk_glfw3_update(screenSize.width, screenSize.height);
}

void SCOpenGLRenderable::beginDrawing()
{
	auto size = getDisplay()->getScreenSize();

	glViewport(0, 0, size.width, size.height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.4, 0.4, 0.4, 1);
}

void SCOpenGLRenderable::endDrawing()
{
	nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);

	const auto glfwDisplay = static_cast<GLFWwindow*>(getDisplay()->getContext());
	assert(glfwDisplay);

	glfwSwapBuffers(glfwDisplay);

}
