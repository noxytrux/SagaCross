#include "SCSettingsScene.hpp"
#include "glTextureLoader.hpp"

using namespace sc;

SCSettingsScene::SCSettingsScene(const std::string &name,
	const std::string rootPath,
	const std::shared_ptr<SCRendererInterface> &renderer,
	const std::shared_ptr<SCAudio> &audio)
	: SCBaseScene(name, rootPath, renderer, audio) 
{
}

SCSettingsScene::~SCSettingsScene() noexcept {

}

SCSceneType SCSettingsScene::Render() {

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
	if (nk_begin(ctx, "Settings", nk_rect(0, 0, screenSize.width, screenSize.height), NK_WINDOW_NO_INPUT)) {

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

		//separator
		nk_layout_row_static(ctx, 50, screenSize.width - margin, 1);
		nk_spacing(ctx, 1);

		//normal buttons

		ctx->style.button.normal = nk_style_item_image(_buttonnormal);
		ctx->style.button.hover = nk_style_item_image(_buttonactive);
		ctx->style.button.active = nk_style_item_image(_buttonactive);

		const auto &resolutions = _renderer->getDisplay()->getResolutions();

		float comboboxsize = 400.0f;
		float labelsize = comboboxsize / (float)screenSize.width;
		float sidesize = (1.0f - labelsize) * 0.5f;

		static const float ratio[] = { sidesize, labelsize, sidesize };

		nk_layout_row(ctx, NK_DYNAMIC, 40, 3, ratio);

		if (_selectedResolution >= resolutions.size()) {
			_selectedResolution = resolutions.size() - 1;
		}

		if (display->allowMultipleResolutions()) {
		
			const auto &curres = resolutions[_selectedResolution];
			 
			std::stringstream ss;

			ss << curres.width << "x" << curres.height;

			nk_spacing(ctx, 1);
			if (nk_combo_begin_label(ctx, ss.str().c_str(), nk_vec2(comboboxsize, 200))) {

				nk_layout_row_dynamic(ctx, 35, 1);

				for (int i = 0; i < resolutions.size(); ++i) {

					std::stringstream().swap(ss);

					const auto &res = resolutions[i];

					ss << res.width << "x" << res.height;

					if (nk_combo_item_label(ctx, ss.str().c_str(), NK_TEXT_CENTERED)) {

						_selectedResolution = i;
					}
				}

				nk_combo_end(ctx);
			}
			nk_spacing(ctx, 1);

			nk_spacing(ctx, 1);
			nk_checkbox_label(ctx, "Fullscreen", &_fullscreenCheckbox);
			nk_spacing(ctx, 1);

			nk_spacing(ctx, 1);
			nk_checkbox_label(ctx, "V-sync", &_vsyncCheckbox);
			nk_spacing(ctx, 1);
		}

		nk_spacing(ctx, 1);
		if (nk_progress(ctx, &_volume, 100, nk_true)) {

			settings->setVolume(_volume / 100.0);
			_audio->setVolume(_volume / 100.0);
		}
		nk_spacing(ctx, 1);

		nk_spacing(ctx, 1);
		nk_checkbox_label(ctx, "Mute", &_muteCheckbox);
		nk_spacing(ctx, 1);

		nk_spacing(ctx, 1);
		nk_checkbox_label(ctx, "Hard AI", &_hardAICheckbox);
		nk_spacing(ctx, 1);

		nk_spacing(ctx, 1);
		nk_checkbox_label(ctx, "Second Camera", &_secondCamCheckbox);
		nk_spacing(ctx, 1);

		//cancel / apply

		float btnsize = 212 / (float)screenSize.width;
		float centersize = ((1.0f - 0.1) - 2.0 * btnsize);

		static const float bottomratio[] = { 0.05, btnsize, centersize, btnsize, 0.05 };

		nk_layout_row(ctx, NK_DYNAMIC, 42, 5, bottomratio);

		nk_spacing(ctx, 1);
		if (nk_button_label(ctx, "CANCEL")) {

			_type = SceneTypeMenu;
		}
		nk_spacing(ctx, 1);
		if (nk_button_label(ctx, "APPLY")) {

			settings->setFullscreen(_fullscreenCheckbox);
			settings->setVsyncEnabled(_vsyncCheckbox);
			settings->setMute(_muteCheckbox);
			settings->setHardAIEnabled(_hardAICheckbox);
			settings->setUseSecondCam(_secondCamCheckbox);

			const auto &selectedResolution = resolutions[_selectedResolution];

			settings->setWidht(selectedResolution.width);
			settings->setHeight(selectedResolution.height);

			settings->save();

			if (display->allowMultipleResolutions()) {

				display->setFullScreen(settings->isFullScreen());
				display->setVsync(settings->isVsyncEnabled());
				_renderer->updateViewPort(selectedResolution);
				display->resizeWindow(selectedResolution);
			}

			if (settings->isMuted()) {
			
				_audio->mute();
			}
			else {
			
				_audio->unmute();
			}
		}
		nk_spacing(ctx, 1);
	}
	nk_end(ctx);

    return _type;
}

void SCSettingsScene::Init() {

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

	const auto &display = _renderer->getDisplay();
	const auto &screenSize = display->getScreenSize();
	const auto &resolutions = _renderer->getDisplay()->getResolutions();

	_selectedResolution = 0;

	for (const auto &res : resolutions) {

		if (res.width == screenSize.width && res.height == screenSize.height) {
			break;
		}

		_selectedResolution++;
	}

	auto settings = _renderer->getSettings();

	_fullscreenCheckbox = settings->isFullScreen();
	_vsyncCheckbox = settings->isVsyncEnabled();
	_muteCheckbox = settings->isMuted();
	_hardAICheckbox = settings->hardAIEnabled();
	_secondCamCheckbox = settings->usesSecondCam();
	_volume = settings->getVolue() * 100;
}

void SCSettingsScene::Destroy() {

	_bgtex = 0;
	_btnntex = 0;
	_btnatex = 0;
	_btnbacktex = 0;

    _type = SceneTypeNone;
}

