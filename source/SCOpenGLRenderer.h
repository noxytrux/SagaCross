#pragma once

#include "SCRendererInterface.h"

namespace sc {

	class SCOpenGLRenderable : public SCRendererInterface {

	public:
		
		SCOpenGLRenderable(const std::shared_ptr<SCDisplay> &display, const std::shared_ptr<SCSettings> &settings);
		virtual ~SCOpenGLRenderable() noexcept;

		virtual void updateViewPort(const SCScreenSize &screenSize) override;
		virtual void beginDrawing() override;
		virtual void endDrawing() override;

		void loadFonts(const std::string &path);

	private:

		struct nk_font_atlas *_atlas;
		struct nk_font *_orbitron30 = nullptr;
		struct nk_font *_orbitron15 = nullptr;
	};
}

