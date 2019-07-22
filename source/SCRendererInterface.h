#pragma once

#include <memory>

#include "SCDisplay.h"
#include "SCSettings.h"
#include "ShaderProgram.hpp"
#include "SCFrustum.hpp"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING

#include "nuklear.h"
#include "glMatrix4x4.hpp"

namespace sc {

	class SCRendererInterface {

	public:
		SCRendererInterface(const std::shared_ptr<SCDisplay> &display, const std::shared_ptr<SCSettings> &settings) 
			: _display(display)
			, _settings(settings)
			, _ctx(nullptr) {
		
            ModelView.loadIdentity();
			Projection.loadIdentity();

			m_vSunVector = xVec3(0, 1, 1);
			m_SunColor = xVec3(1.000000, 0.598512, 0.157620);
		}
		
		virtual ~SCRendererInterface() noexcept {}

		const std::shared_ptr<SCDisplay> &getDisplay() const { return _display; }
		const std::shared_ptr<SCSettings> &getSettings() const { return _settings; }
		const SCFrustum& getFrustum() const { return _frustum; }

		struct nk_context *getUIContext() const { return _ctx; }

		std::vector<struct nk_font *> getFontList() const { return _fontlist;  }

		virtual void updateViewPort(const SCScreenSize &screenSize) = 0;
		virtual void beginDrawing() = 0;
		virtual void endDrawing() = 0;

		glMatrix4x4 ModelView;
		glMatrix4x4 Projection;

		//TODO: get rid of this ?
		xVec3 m_vSunVector;
		xVec3 m_SunColor;

	protected:

		std::shared_ptr<SCDisplay> _display;
		std::shared_ptr<SCSettings> _settings;
		struct nk_context *_ctx;
		std::vector<struct nk_font*> _fontlist;

		SCFrustum _frustum;
	};
}

