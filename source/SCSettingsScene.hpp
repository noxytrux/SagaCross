#pragma once

#include "SCBaseScene.hpp"

namespace sc {

	class SCSettingsScene final : public SCBaseScene {

	public:

		SCSettingsScene(const std::string &name,
						const std::string rootPath,
						const std::shared_ptr<SCRendererInterface> &renderer,
						const std::shared_ptr<SCAudio> &audio);
		
		virtual ~SCSettingsScene() noexcept;

		virtual SCSceneType Render() override;

		virtual void Init() override;
		virtual void Destroy() override;

	private:

		GLuint _bgtex;
		GLuint _btnntex;
		GLuint _btnatex;
		GLuint _btnbacktex;

		struct nk_image _windowbg;
		struct nk_image _buttonnormal;
		struct nk_image _buttonactive;
		struct nk_image _backbtn;

		uint32_t _selectedResolution;

		int _fullscreenCheckbox;
		int _vsyncCheckbox;
		int _muteCheckbox;
		int _hardAICheckbox;
		int _secondCamCheckbox;

		nk_size _volume;
	};
}
