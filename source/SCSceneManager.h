#pragma once

#include "SCNonCopyable.hpp"
#include "SCBaseScene.hpp"
#include "SCAudio.h"

namespace sc {

	class SCSceneManager final : public SCNonCopyable {
	public:
	
		SCSceneManager(	const std::string rootPath, 
						const std::shared_ptr<SCRendererInterface> &renderer, 
						const std::shared_ptr<SCAudio> &audio);
		~SCSceneManager() noexcept;

		SCSceneType Render();

		bool setCurrent(const std::string &name);

		std::shared_ptr<SCBaseScene> getScenePointer(const std::string & name) const;
		const std::shared_ptr<SCBaseScene> &getCurrentScenePointer() const { return _currentScene; }

	private:

		std::shared_ptr<SCBaseScene> _currentScene;
		std::map<std::string, std::shared_ptr<SCBaseScene>> _scenes;
	};
}

