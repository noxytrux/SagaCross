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

	};
}
