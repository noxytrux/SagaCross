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

GLuint tex = 0;
std::shared_ptr<SCFastMesh> model = nullptr;

std::shared_ptr<SCMeshInstance> MeshManager = nullptr;
std::shared_ptr<SCCamera> camera = nullptr;

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
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto renderer = std::dynamic_pointer_cast<SCOpenGLRenderable>(_renderer);
    _renderer->Projection.mPerspective((45.0f * M_PI) / 180.0f, (float)screenSize.width / (float)screenSize.height, 0.001f, 50000.0f);

    //camera->Apply();
    camera->FreeCam(renderer->getDisplay());

    auto frustum = _renderer->getFrustum();

    float proj[16];
    float mv[16];

    _renderer->Projection.getColumnMajor44(proj);
    _renderer->ModelView.getColumnMajor44(mv);

    frustum.modelview = mv;
    frustum.projection = proj;

    frustum.calculateFrustum();

    auto current = renderer->SimpleShader;
    current->begin();

    glBindTexture(GL_TEXTURE_2D, tex);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(current->uniforms[UNI_TEX0], 0);
    glUniform4f(current->uniforms[UNI_TEX1], 1.0f, 1.0f, 1.0f, 1.0f);

//    glDisable(GL_CULL_FACE);

    model->Draw();

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

    camera = std::make_shared<SCCamera>(_renderer, 0, 1, 0 );
    MeshManager = std::make_shared<SCMeshInstance>(_renderer);

    tex = textureLoader.loadFile(_rootPath + "maps/map1/level001_color.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
	model = MeshManager->GetMesh("maps/map1/level001_terrain", _rootPath, SCMeshTypeNormal);
    model->setScale(0.01);
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

