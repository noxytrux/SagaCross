#include "SCDebugScene.hpp"
#include "glTextureLoader.hpp"

#include "SCBulletManager.h"
#include "SCParticleLayer.h"
#include "SCMeshInstanceManager.h"
#include "SCVegetation.h"
#include "SCCamera.h"
#include "SCGround.h"
#include "glMatrix4x4.hpp"

using namespace sc;

SCDebugScene::SCDebugScene(const std::string &name,
	const std::string rootPath,
	const std::shared_ptr<SCRendererInterface> &renderer,
	const std::shared_ptr<SCAudio> &audio)
: SCBaseScene(name, rootPath, renderer, audio)
{

}

SCDebugScene::~SCDebugScene() noexcept {

}

SCSceneType SCDebugScene::Render() {

	auto ctx = _renderer->getUIContext();
	auto display = _renderer->getDisplay();
	auto screenSize = display->getScreenSize();

	//3D



    //ui
    ctx->style.window.fixed_background = nk_style_item_color(nk_rgba(0, 0, 0, 0));

    if (nk_begin(ctx, "Render", nk_rect(0, 0, screenSize.width, screenSize.height), NK_WINDOW_NO_INPUT | NK_WINDOW_NO_SCROLLBAR)) {

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

        //TODO: add here pads for mobile UI
    }
    nk_end(ctx);

    return _type;
}

void SCDebugScene::Init() {

    //clean up before loading game
    textureLoader.releaseTextures();

    auto btnbackpath = _rootPath + "textures/menubtn.png";
    _btnbacktex = textureLoader.loadFile(btnbackpath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    _backbtn = nk_subimage_id(_btnbacktex, 32, 32, nk_rect(0, 0, 32, 32));

	//get game info 
	auto settings = _renderer->getSettings();
	
}

void SCDebugScene::Destroy() {

    _btnbacktex = 0;
    
	//clean up after game
	textureLoader.releaseTextures();

    _type = SceneTypeNone;
}

void SCDebugScene::handleScroll(double s) {
    // ...
}

void SCDebugScene::handleMouse(int button, int action, double x, double y) {

}

void SCDebugScene::handleMove(const xVec2 &direction, const float angle) {

}

