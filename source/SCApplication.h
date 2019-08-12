#pragma once

#include "Core-pch.hpp"
#include "SCRendererInterface.h"
#include "SCInputInterface.h"
#include "SCSettings.h"
#include "SCSceneManager.h"
#include "SCAudio.h"

namespace sc {

	class SCApplication final {
	
	public:
	
        SCApplication(const std::shared_ptr<SCSettings> &settings, const std::string &rootPath);
		~SCApplication() noexcept;
	
		int run();

		void setShouldUseRunLoop(bool userunloop) { _userunloop = userunloop; }

		const std::shared_ptr<SCInputInteface> &getInput() const { return _input; }
        bool isRenderingGame() const;

	private:

		const std::string getResourcePath() const;

	#ifdef __EMSCRIPTEN__
	public:
	#endif
		void renderFrame();
		
	private:

		std::shared_ptr<SCSettings> _settings;
        std::string _resourcePath;
		std::shared_ptr<SCRendererInterface> _renderer;
		bool _userunloop;
		std::shared_ptr<SCInputInteface> _input;
		std::shared_ptr<SCAudio> _audio;
		std::unique_ptr<SCSceneManager> _sceneManager;
		struct nk_vec2 scroll;
	};
}
