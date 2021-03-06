#pragma once

#include "SCBaseScene.hpp"
#include "GameTimer.hpp"

namespace sc {

	class SCDebugScene final : public SCBaseScene {

	public:

		SCDebugScene(const std::string &name,
					 const std::string rootPath,
					 const std::shared_ptr<SCRendererInterface> &renderer,
					 const std::shared_ptr<SCAudio> &audio);
		
		virtual ~SCDebugScene() noexcept;

		virtual SCSceneType Render() override;

		virtual void Init() override;
		virtual void Destroy() override;

		virtual void handleScroll(double s) override;
		virtual void handleMouse(int button, int action, double x, double y) override;
		virtual void handleMove(const xVec2 &direction, const float angle) override;

	private:

        GLuint _btnbacktex;
        struct nk_image _backbtn;
	};
}
