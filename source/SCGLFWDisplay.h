#pragma once

#include "Core-pch.hpp"
#include "SCDisplay.h"

namespace sc {

	class SCGLFWDisplay final : public SCDisplay {
	public:

		SCGLFWDisplay(const uint32_t width, const uint32_t height, const bool fullscreen);
		virtual ~SCGLFWDisplay() noexcept;

		virtual void makeWindow() override;
		virtual const bool shouldClose() const override;

		virtual const bool allowMultipleResolutions() const override { 
		#ifdef __EMSCRIPTEN__
			return false;
		#else
			return true; 
		#endif
		}

		virtual void resizeWindow(const SCScreenSize &size) override;
		virtual void resizeWindow(uint32_t width, uint32_t height) override;
		virtual const std::vector<SCScreenSize> getResolutions() override;
		virtual void setFullScreen(bool fullscreen) override;
		virtual void* getContext() const override { return _win; }
		virtual void setVsync(const bool vsync) override;

	private:

		GLFWwindow *_win;
	};
}
