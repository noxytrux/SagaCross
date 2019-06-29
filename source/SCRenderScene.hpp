#pragma once

#include "SCBaseScene.hpp"

namespace sc {

	class SCRenderScene final : public SCBaseScene {

	public:

		SCRenderScene(const std::string &name,
					  const std::string rootPath,
					  const std::shared_ptr<SCRendererInterface> &renderer,
					  const std::shared_ptr<SCAudio> &audio);
		
		virtual ~SCRenderScene() noexcept;

		virtual SCSceneType Render() override;

		virtual void Init() override;
		virtual void Destroy() override;

		virtual bool isRender() override { return true; }

		virtual void handleScroll(double s) override;
		virtual void handleMouse(int button, int action, double x, double y) override;
		virtual void handleMove(const xVec2 &direction, const float angle) override;

	private:

	};
}
