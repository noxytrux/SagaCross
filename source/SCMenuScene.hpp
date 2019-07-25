#pragma once

#include "SCBaseScene.hpp"

namespace sc {

	class SCMenuScene final : public SCBaseScene {

	public:

		SCMenuScene(const std::string &name,
					const std::string rootPath,
					const std::shared_ptr<SCRendererInterface> &renderer,
					const std::shared_ptr<SCAudio> &audio);
	
		virtual ~SCMenuScene() noexcept;

		virtual SCSceneType Render() override;

		virtual void Init() override;
		virtual void Destroy() override;

	private:

		GLuint _bgtex;
		GLuint _btnntex;
		GLuint _btnatex;

		struct nk_image _windowbg;
		struct nk_image _buttonnormal;
		struct nk_image _buttonactive;
	};

}