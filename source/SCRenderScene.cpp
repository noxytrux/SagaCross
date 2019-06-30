#include "SCRenderScene.hpp"
#include "glTextureLoader.hpp"

using namespace sc;

SCRenderScene::SCRenderScene(const std::string &name,
	const std::string rootPath,
	const std::shared_ptr<SCRendererInterface> &renderer,
	const std::shared_ptr<SCAudio> &audio)
: SCBaseScene(name, rootPath, renderer, audio)
{

}

SCRenderScene::~SCRenderScene() {

}

SCSceneType SCRenderScene::Render() {

    return _type;
}

void SCRenderScene::Init() {

	//clean up before loading game
	textureLoader.releaseTextures();

	//get game info 
	auto settings = _renderer->getSettings();
	auto &defaults = settings->userDefaults;

	auto selectedmap = defaults.value<int>("map", 0);
	auto botscount = defaults.value<int>("botscount", 0);

	defaults.clear();


}

void SCRenderScene::Destroy() {

	//clean up after game
	textureLoader.releaseTextures();

    _type = SceneTypeNone;
}

void SCRenderScene::handleScroll(double s) {

}

void SCRenderScene::handleMouse(int button, int action, double x, double y) {
   
}

void SCRenderScene::handleMove(const xVec2 &direction, const float angle) {

}
