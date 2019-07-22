#include "SCSceneManager.h"

#include "SCMenuScene.hpp"
#include "SCNewGameScene.hpp"
#include "SCSettingsScene.hpp"
#include "SCRenderScene.hpp"
#include "SCDebugScene.hpp"

using namespace sc;

SCSceneManager::SCSceneManager(const std::string rootPath, const std::shared_ptr<SCRendererInterface> &renderer, const std::shared_ptr<SCAudio> &audio)
	: _currentScene(nullptr)
{
	auto menuScene	   = std::make_shared<SCMenuScene>("menu", rootPath, renderer, audio);
	auto newGameScene  = std::make_shared<SCNewGameScene>("newgame", rootPath, renderer, audio);
	auto settingsScene = std::make_shared<SCSettingsScene>("settings", rootPath, renderer, audio);
	auto renderScene   = std::make_shared<SCRenderScene>("render", rootPath, renderer, audio);
	auto debugScene	   = std::make_shared<SCDebugScene>("debug", rootPath, renderer, audio);

	_scenes[menuScene->getName()] = menuScene;
	_scenes[newGameScene->getName()] = newGameScene;
	_scenes[settingsScene->getName()] = settingsScene;
	_scenes[renderScene->getName()] = renderScene;
	_scenes[debugScene->getName()] = debugScene;

	this->setCurrent("menu");
}

SCSceneManager::~SCSceneManager() noexcept
{
	for (auto it = _scenes.begin(); it != _scenes.end(); it++) {

		if ((*it).second != nullptr) {

			(*it).second->Destroy();
		}

		(*it).second = nullptr;
	}

	_scenes.clear();
}

SCSceneType SCSceneManager::Render() {
	
	return _currentScene->Render();
}

bool SCSceneManager::setCurrent(const std::string & name) {


	auto ptr = getCurrentScenePointer();
	
	if (ptr) {
		ptr->Destroy();
	}

	auto it = _scenes.find(name);

	if (it == _scenes.end()) {

		return false;
	}

	if (_currentScene != it->second) {

		_currentScene = it->second;
	}

	auto nptr = getCurrentScenePointer();

	if (nptr) {
		nptr->Init();
	}

	return true;
}

std::shared_ptr<SCBaseScene> SCSceneManager::getScenePointer(const std::string &name) const {

	auto it = _scenes.find(name);

	if (it == _scenes.end()) {

		return nullptr;
	}

	return it->second;
}
