#include "SCNewGameScene.hpp"
#include "glTextureLoader.hpp"

using namespace sc;

SCNewGameScene::SCNewGameScene(const std::string &name,
	const std::string rootPath,
	const std::shared_ptr<SCRendererInterface> &renderer,
	const std::shared_ptr<SCAudio> &audio)
	: SCBaseScene(name, rootPath, renderer, audio) 
{
}

SCNewGameScene::~SCNewGameScene() {

}

SCSceneType SCNewGameScene::Render() {

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

	auto settings = _renderer->getSettings();

	//background
	ctx->style.window.fixed_background = nk_style_item_image(_windowbg);
	if (nk_begin(ctx, "NewGame", nk_rect(0, 0, screenSize.width, screenSize.height), NK_WINDOW_NO_INPUT)) {

		auto fonts = _renderer->getFontList();
		auto margin = 30;

		//buttons
		nk_style_set_font(ctx, &fonts[1]->handle);

		//backbutton
		ctx->style.button.normal = nk_style_item_image(_backbtn);
		ctx->style.button.hover = nk_style_item_image(_backbtn);
		ctx->style.button.active = nk_style_item_image(_backbtn);

		nk_layout_row_static(ctx, 32, 32, 1);
		if (nk_button_label(ctx, "")) {

			_type = SceneTypeMenu;
		}

	

	
	}
	nk_end(ctx);

    return _type;
}

void SCNewGameScene::Init() {

	auto bgpath = _rootPath + "menubg.jpg";
	auto btnnpath = _rootPath + "gui_screen-button.png";
	auto btnapath = _rootPath + "gui_screen-button-p.png";
	auto btnbackpath = _rootPath + "textures/menubtn.png";

	_bgtex = textureLoader.loadFile(bgpath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
	_btnntex = textureLoader.loadFile(btnnpath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
	_btnatex = textureLoader.loadFile(btnapath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
	_btnbacktex = textureLoader.loadFile(btnbackpath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);

	_buttonnormal = nk_subimage_id(_btnntex, 212, 42, nk_rect(0, 0, 212, 42));
	_buttonactive = nk_subimage_id(_btnatex, 212, 42, nk_rect(0, 0, 212, 42));
	_backbtn = nk_subimage_id(_btnbacktex, 32, 32, nk_rect(0, 0, 32, 32));

	auto ctx = _renderer->getUIContext();

	ctx->style.button.text_normal = nk_rgb(0, 0, 0);
	ctx->style.button.text_hover = nk_rgb(0, 0, 0);
	ctx->style.button.text_active = nk_rgb(0, 0, 0);

	ctx->style.text.color = nk_rgb(0, 0, 0);
}

void SCNewGameScene::Destroy() {

	_bgtex = 0;
	_btnntex = 0;
	_btnatex = 0;
	_btnbacktex = 0;

    _type = SceneTypeNone;
}
