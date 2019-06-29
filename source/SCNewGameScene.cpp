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

  

    return _type;
}

void SCNewGameScene::Init() {

   
}

void SCNewGameScene::Destroy() {

    _type = SceneTypeNone;
}
