#pragma once

#include "SCBaseScene.hpp"

namespace sc {

	class SCNewGameScene final : public SCBaseScene {

	public:

		SCNewGameScene(const std::string &name,
					   const std::string rootPath,
					   const std::shared_ptr<SCRendererInterface> &renderer,
					   const std::shared_ptr<SCAudio> &audio);

		virtual ~SCNewGameScene() noexcept;

		virtual SCSceneType Render() override;

		virtual void Init() override;
		virtual void Destroy() override;

	private:

		GLuint _bgtex;
		GLuint _btnntex;
		GLuint _btnatex;
		GLuint _btnbacktex;
		GLuint _leftbtntex;
		GLuint _rightbtntex;

		struct nk_image _windowbg;
		struct nk_image _buttonnormal;
		struct nk_image _buttonactive;
		struct nk_image _backbtn;
		struct nk_image _lhsbtn;
		struct nk_image _rhsbtn;

		std::vector<struct nk_image> _maps;
		std::vector<std::string> _names;

		int _selectedmap;
		int _botscount;
	};
}
