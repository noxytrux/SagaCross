#include "SCNewGameScene.hpp"
#include "glTextureLoader.hpp"

using namespace sc;

SCNewGameScene::SCNewGameScene(const std::string &name,
	const std::string rootPath,
	const std::shared_ptr<SCRendererInterface> &renderer,
	const std::shared_ptr<SCAudio> &audio)
	: SCBaseScene(name, rootPath, renderer, audio) 
{
	_selectedmap = 0;
	_botscount = 3;
}

SCNewGameScene::~SCNewGameScene() noexcept {

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
	if (nk_begin(ctx, "NewGame", nk_rect(0, 0, screenSize.width, screenSize.height), NK_WINDOW_NO_INPUT | NK_WINDOW_NO_SCROLLBAR)) {

		auto fonts = _renderer->getFontList();

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

		const float mapsize = 400.0f / screenSize.width;
        float sepsize = (1.0f - mapsize) * 0.5f;
	
		static const float selectmapratio[] = { sepsize, mapsize };
		nk_layout_row(ctx, NK_DYNAMIC, 300, 2, selectmapratio);

		auto image = nk_style_item_image(_maps[_selectedmap]);

		//map
		ctx->style.button.normal = image;
		ctx->style.button.hover = image;
		ctx->style.button.active = image;

		nk_spacing(ctx, 1);
		nk_button_label(ctx, "");
	
		const float labelsize = 200.0f / screenSize.width;
		const float btnsize = 64.0f / screenSize.width;
		const float margin = (1.0f - (labelsize + (2.0f * btnsize))) * 0.5f;

		static const float selectlabelratio[] = { margin, btnsize, labelsize , btnsize };
		nk_layout_row(ctx, NK_DYNAMIC, 64, 4, selectlabelratio);
		
		nk_spacing(ctx, 1);
		
		//left 
		ctx->style.button.normal = nk_style_item_image(_lhsbtn);
		ctx->style.button.hover = nk_style_item_image(_lhsbtn);
		ctx->style.button.active = nk_style_item_image(_lhsbtn);
		
		if (nk_button_label(ctx, "")) {

			_selectedmap++;
			_selectedmap %= _maps.size();
		}

		nk_label(ctx, _names[_selectedmap].c_str(), NK_TEXT_ALIGN_CENTERED | NK_TEXT_ALIGN_MIDDLE);

		//right 
		ctx->style.button.normal = nk_style_item_image(_rhsbtn);
		ctx->style.button.hover = nk_style_item_image(_rhsbtn);
		ctx->style.button.active = nk_style_item_image(_rhsbtn);

		if (nk_button_label(ctx, "")) {

			_selectedmap++;
			_selectedmap %= _maps.size();
		}

		nk_layout_row_static(ctx, 64, screenSize.width, 1);

		nk_label(ctx, "BOTS", NK_TEXT_ALIGN_CENTERED | NK_TEXT_ALIGN_MIDDLE);

		nk_layout_row(ctx, NK_DYNAMIC, 64, 4, selectlabelratio);

		nk_spacing(ctx, 1);

		//left 
		ctx->style.button.normal = nk_style_item_image(_lhsbtn);
		ctx->style.button.hover = nk_style_item_image(_lhsbtn);
		ctx->style.button.active = nk_style_item_image(_lhsbtn);

		if (nk_button_label(ctx, "")) {

			_botscount--;
			_botscount = clamp(_botscount, 3, 15);
		}

		nk_label(ctx, std::to_string(_botscount).c_str(), NK_TEXT_ALIGN_CENTERED | NK_TEXT_ALIGN_MIDDLE);

		//right 
		ctx->style.button.normal = nk_style_item_image(_rhsbtn);
		ctx->style.button.hover = nk_style_item_image(_rhsbtn);
		ctx->style.button.active = nk_style_item_image(_rhsbtn);

		if (nk_button_label(ctx, "")) {

			_botscount++;
			_botscount = clamp(_botscount, 3, 15);
		}

		//start
		ctx->style.button.normal = nk_style_item_image(_buttonnormal);
		ctx->style.button.hover = nk_style_item_image(_buttonactive);
		ctx->style.button.active = nk_style_item_image(_buttonactive);

		const float startsize = 212.0f / screenSize.width;
		sepsize = (1.0f - startsize) * 0.5f;

		static const float startratio[] = { sepsize, startsize };
		nk_layout_row(ctx, NK_DYNAMIC, 42, 2, startratio);

		nk_spacing(ctx, 1);

		if (nk_button_label(ctx, "START")) {

			auto &defaults = settings->userDefaults;

			defaults["map"] = _selectedmap + 1;
			defaults["botscount"] = _botscount;

			_type = SceneTypeRender;
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
	auto lhsbtnpath = _rootPath + "leftbtn.png";
	auto rhsbtnpath = _rootPath + "rightbtn.png";

	_bgtex = textureLoader.loadFile(bgpath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
	_btnntex = textureLoader.loadFile(btnnpath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
	_btnatex = textureLoader.loadFile(btnapath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
	_btnbacktex = textureLoader.loadFile(btnbackpath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
	_leftbtntex = textureLoader.loadFile(lhsbtnpath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
	_rightbtntex = textureLoader.loadFile(rhsbtnpath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);

	_buttonnormal = nk_subimage_id(_btnntex, 212, 42, nk_rect(0, 0, 212, 42));
	_buttonactive = nk_subimage_id(_btnatex, 212, 42, nk_rect(0, 0, 212, 42));
	_backbtn = nk_subimage_id(_btnbacktex, 32, 32, nk_rect(0, 0, 32, 32));
	_lhsbtn = nk_subimage_id(_leftbtntex, 64, 64, nk_rect(0, 0, 64, 64));
	_rhsbtn = nk_subimage_id(_rightbtntex, 64, 64, nk_rect(0, 0, 64, 64));

	//maps 
	std::vector<std::string> paths = {
		{ _rootPath + "artworks/gui_artwork_level001-03.jpeg" },
		{ _rootPath + "artworks/gui_artwork_level002-03.jpeg" }
	};

	for (const auto &path : paths) {
	
		auto glid = textureLoader.loadFile(path, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
		auto nkid = nk_subimage_id(glid, 400, 300, nk_rect(0, 0, 400, 300));

		_maps.push_back(nkid);
	}

	_names.push_back("Forest");
	_names.push_back("Greece");

	_selectedmap = 0;
	_botscount = 3;

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

	_maps.clear();
	_names.clear();

    _type = SceneTypeNone;
}
