#pragma once

#include "Core-pch.hpp"
#include "EnumOptionMacro.hpp"
#include "SCRendererInterface.h"
#include "SCAudio.h"

namespace sc {

	typedef SC_ENUM(unsigned int, SCSceneType) {

		SceneTypeMenu,
		SceneTypeNewGame,
		SceneTypeSettings,
		SceneTypeRender,
		SceneTypeMultiplayer,
		SceneTypeExit,
		SceneTypeNone
	};

	class SCBaseScene : public SCNonCopyable {

	public:

		SCBaseScene(const std::string &name, 
					const std::string rootPath, 
					const std::shared_ptr<SCRendererInterface> &renderer,
					const std::shared_ptr<SCAudio> &audio)
			: _name(name)
			, _rootPath(rootPath)
			, _type(SceneTypeNone) 
			, _renderer(renderer) 
			, _audio(audio)
		{
		}

		virtual ~SCBaseScene() noexcept = default;

		std::string const & getName() const { return _name; }

		virtual SCSceneType Render() = 0;

		virtual void Init() = 0;
		virtual void Destroy() = 0;

		virtual bool isRender() { return false; }

		virtual void handleScroll(double s) {}
		virtual void handleMouse(int button, int action, double x, double y) {}
		virtual void handleMove(const xVec2 &direction, const float angle) {}

        virtual bool isRenderingGame() { return false; }

	protected:

		std::string _name;
		std::string _rootPath;
		SCSceneType _type;
		std::shared_ptr<SCRendererInterface> _renderer;
		std::shared_ptr<SCAudio> _audio;
	};

}
