#include "SCMenuScene.hpp"
#include "glTextureLoader.hpp"

using namespace sc;

SCMenuScene::SCMenuScene(const std::string &name,
	const std::string rootPath,
	const std::shared_ptr<SCRendererInterface> &renderer,
	const std::shared_ptr<SCAudio> &audio)
	: SCBaseScene(name, rootPath, renderer, audio) 
{

}

SCMenuScene::~SCMenuScene() noexcept {

}

SCSceneType SCMenuScene::Render() {

	//TODO: refactor this code, each scene use same bg logic with window this is not DRY
	auto ctx = _renderer->getUIContext();
	auto display = _renderer->getDisplay();
	auto screenSize = display->getScreenSize();

	const float defaultWidth = 1920;
	const float defaultHeight = 1080;

	const float scale = screenSize.width / defaultWidth;

	float trimWidth = defaultWidth;
	float xOffset = 0;

	if (scale < 1.0) {
		trimWidth *= scale;
		xOffset = (defaultWidth - trimWidth) * 0.5;
	}

	//trim if needed
	_windowbg = nk_subimage_id(_bgtex, defaultWidth, defaultHeight, 
		nk_rect(xOffset,
				0, 
				std::fmin(defaultWidth, screenSize.width), 
				std::fmin(defaultHeight, screenSize.height))
	);

    //background
    ctx->style.window.fixed_background = nk_style_item_image(_windowbg);
    if (nk_begin(ctx, "MainMenu", nk_rect(0, 0, screenSize.width, screenSize.height), NK_WINDOW_NO_INPUT | NK_WINDOW_NO_SCROLLBAR)) {
		
		auto fonts = _renderer->getFontList();
		auto margin = 30;

		//separator
		nk_layout_row_static(ctx, 80, screenSize.width - margin, 1);
		nk_spacing(ctx, 1);

		//logo
		nk_style_set_font(ctx, &fonts[0]->handle);
		nk_layout_row_static(ctx, 90, screenSize.width - margin, 1);
		nk_label(ctx, "SAGA CROSS", NK_TEXT_ALIGN_CENTERED);

		//buttons
		nk_style_set_font(ctx, &fonts[1]->handle);
        
		float btnsize = 425.0f / (float)screenSize.width;
		float sidesize = (1.0f - btnsize) * 0.5f;

		static const float ratio[] = { sidesize, btnsize, sidesize };

        nk_layout_row(ctx, NK_DYNAMIC, 85, 3, ratio);
       
		nk_spacing(ctx, 1);
        if (nk_button_label(ctx, "NEW GAME")) {

            _type = SceneTypeNewGame;
        }
        nk_spacing(ctx, 1);

#ifndef __EMSCRIPTEN__
		nk_spacing(ctx, 1);
		if (nk_button_label(ctx, "MULTIPLAYER")) {

			//TODO: implement multiplayer
		}
		nk_spacing(ctx, 1);
#endif		

        nk_spacing(ctx, 1);
        if (nk_button_label(ctx, "SETTINGS")) {

            _type = SceneTypeSettings;
        }
        nk_spacing(ctx, 1);

#ifndef MOBILE
		nk_spacing(ctx, 1);
		if (nk_button_label(ctx, "EXIT")) {

			_type = SceneTypeExit;
		}
		nk_spacing(ctx, 1);
#endif

    }
    nk_end(ctx);

    return _type;
}

void SCMenuScene::Init() {

    auto bgpath = _rootPath + "menubg.jpg";
    auto btnnpath = _rootPath + "gui_screen-button.png";
    auto btnapath = _rootPath + "gui_screen-button-p.png";

    _bgtex   = textureLoader.loadFile(bgpath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    _btnntex = textureLoader.loadFile(btnnpath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    _btnatex = textureLoader.loadFile(btnapath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);

	_buttonnormal = nk_subimage_id(_btnntex, 425, 85, nk_rect(0, 0, 425, 85));
    _buttonactive = nk_subimage_id(_btnatex, 425, 85, nk_rect(0, 0, 425, 85));

	auto ctx = _renderer->getUIContext();

    ctx->style.button.normal = nk_style_item_image(_buttonnormal);
    ctx->style.button.hover  = nk_style_item_image(_buttonactive);
    ctx->style.button.active = nk_style_item_image(_buttonactive);

    ctx->style.button.text_normal = nk_rgb(0, 0, 0);
    ctx->style.button.text_hover  = nk_rgb(0, 0, 0);
    ctx->style.button.text_active = nk_rgb(0, 0, 0);

	ctx->style.text.color = nk_rgb(0, 0, 0);

	auto audio = _audio;

	audio->loadMusic("music/menu.mp3");
	audio->playMusic();
}

void SCMenuScene::Destroy() {

    _bgtex = 0;
    _btnntex = 0;
    _btnatex = 0;

    _type = SceneTypeNone;
}

