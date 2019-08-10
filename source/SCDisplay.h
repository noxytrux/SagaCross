#pragma once

#include "Core-pch.hpp"

namespace sc {

	struct SCScreenSize {

		uint32_t width;
		uint32_t height;

		bool operator < (const SCScreenSize &other) const 
		{ 
			return width < other.width && height < other.height; 
		}
	};

	class SCDisplay : public SCNonCopyable {
	public:

		SCDisplay(const uint32_t width, const uint32_t height, const bool fullscreen);
		virtual ~SCDisplay() noexcept;

		virtual void makeWindow() = 0;
		virtual const bool shouldClose() const = 0;
		virtual const bool allowMultipleResolutions() const = 0;
		virtual void resizeWindow(const SCScreenSize &size) = 0;
		virtual void resizeWindow(uint32_t width, uint32_t height) = 0;
		virtual void* getContext() const { return nullptr; }
		virtual void setVsync(const bool vsync) = 0;

		const SCScreenSize & getScreenSize() const { return _size; }
		bool isSmallDisplay() const { return _size.width < 1024 || _size.height < 768; }

		virtual const std::vector<SCScreenSize> getResolutions() = 0;

		virtual void setFullScreen(bool fullscreen) { _fullscreen = fullscreen; }
		bool isFullScreen() const { return _fullscreen; }

	protected:

		SCScreenSize _size;
		bool _fullscreen;
	};
}
