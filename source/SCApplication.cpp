#include "SCApplication.h"
#include "SCOpenGLRenderer.h"
#include "SCGLFWDisplay.h"
#include "SCGLFWInput.h"

using namespace sc;

static std::shared_ptr<SCDisplay> makeDisplay(const std::shared_ptr<SCSettings> &settings)
{
#ifdef MOBILE

#else 
	return std::make_shared<SCGLFWDisplay>(settings->getWidht(), settings->getHeight(), settings->isFullScreen());
#endif 
}

static std::shared_ptr<SCInputInteface> makeInput(const std::shared_ptr<SCDisplay> &display)
{
#ifdef MOBILE

#else 
	return std::make_shared<SCGLFWInput>(display->getContext());
#endif 
}

SCApplication::SCApplication() 
	: _settings(nullptr)
	, _renderer(nullptr)
	, _userunloop(true)
	, _input(nullptr)
{
	auto filename = getResourcePath() + "settings.bin";

	_settings = std::make_shared<SCSettings>(filename);

	if (!_settings->load()) {
		_settings->save();
	}

	auto display = makeDisplay(_settings);

	display->makeWindow();
	display->setVsync(_settings->isVsyncEnabled());

	auto renderer = std::make_shared<SCOpenGLRenderable>(display, _settings, getResourcePath());
	renderer->loadFonts(getResourcePath());

	_renderer = renderer;

	_input = makeInput(display);
	_audio = std::make_shared<SCAudio>(getResourcePath());

	_audio->setVolume(_settings->getVolue());

	if (_settings->isMuted()) {
		_audio->mute();
	}

	_sceneManager = std::make_unique<SCSceneManager>(getResourcePath(), _renderer, _audio);
}

SCApplication::~SCApplication()
{

}

const std::string SCApplication::getResourcePath()
{

#if defined(__linux__)

	char result[PATH_MAX];

	ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
	std::string path = std::string(result, (count > 0) ? count : 0);

	return path.substr(0, path.find_last_of("\\/")) + "/resource/";

#else 

	return "resource/";

#endif

}

void SCApplication::renderFrame()
{
#ifdef __EMSCRIPTEN__
	_audio->update();
#endif

	auto ctx = _renderer->getUIContext();

	nk_input_begin(ctx);

	const auto &pos = _input->getMousePosition();
	const auto &doublePos = _input->getDoubleClickPosition();
	auto x = static_cast<int>(pos.x);
	auto y = static_cast<int>(pos.y);

	_input->update();

	nk_input_motion(ctx, x, y);
	nk_input_button(ctx, NK_BUTTON_LEFT, x, y, _input->isLeftMouseButtonDown());
	nk_input_button(ctx, NK_BUTTON_MIDDLE, x, y, _input->isMiddleMouseButtonDown());
	nk_input_button(ctx, NK_BUTTON_RIGHT, x, y, _input->isRightMouseButtonDown());
	nk_input_button(ctx, NK_BUTTON_DOUBLE, doublePos.x, doublePos.y, _input->isDoubleClickDown());
	nk_input_scroll(ctx, scroll);
	nk_input_end(ctx);

	scroll = nk_vec2(0, 0);

	_renderer->beginDrawing();

	switch (_sceneManager->Render()) {

	case SceneTypeMenu: {

		_sceneManager->setCurrent("menu");
	}
		break;

	case SceneTypeNewGame: {

		_sceneManager->setCurrent("newgame");
	}
		break;

	case SceneTypeSettings: {

		_sceneManager->setCurrent("settings");
	}
		break;

	case SceneTypeRender: {

		_sceneManager->setCurrent("render");
	}
		break;

	case SceneTypeNone: {

		_sceneManager->getCurrentScenePointer()->handleMouse(-1, 0, x, y);
	}
		break;

	case SceneTypeExit: {
	
		_userunloop = false;
	}
		break;

	default:
		break;
	}

	_renderer->endDrawing();
}

int SCApplication::run() 
{
	auto ctx = _renderer->getUIContext();
	scroll = nk_vec2(0, 0);

	_input->movementCallback = [&](const xVec2 &direction, const float angle) {
	
		_sceneManager->getCurrentScenePointer()->handleMove(direction, angle);
	};

	_input->mouseCallback = [&](const int button, const int action, const xVec2 &pos) {
	
		_sceneManager->getCurrentScenePointer()->handleMouse(button, action, pos.x, pos.y);
	};

	_input->textCallback = [&](unsigned int codepoint) {
	
		nk_input_unicode(ctx, codepoint);
	};

	_input->scrollCallback = [&](const xVec2 &s) {
	
		scroll.x += s.x;
		scroll.y += s.y;
	};

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(renderFrame, 0, 1);
#else
	auto display = _renderer->getDisplay();

	while (!display->shouldClose() && _userunloop) {
	
		renderFrame();
	}
#endif

	return EXIT_SUCCESS;
}