#pragma once

#include "Core-pch.hpp"

namespace sc {

	class SCInputInteface : public SCNonCopyable {

	public:

		SCInputInteface(void *context) : _context(context) {}
		virtual ~SCInputInteface() noexcept = default;

		virtual void update() = 0;

		virtual xVec2 getMousePosition() = 0;
		virtual xVec2 getDoubleClickPosition() = 0;
		
		virtual bool isLeftMouseButtonDown() = 0;
		virtual bool isRightMouseButtonDown() = 0;
		virtual bool isMiddleMouseButtonDown() = 0;
		virtual bool isDoubleClickDown() = 0;

		const void *getContext() const { return _context; }

	public:

		std::function<void(const xVec2 &direction, const float angle)> movementCallback = nullptr;
		std::function<void(const int button, const int action, const xVec2 &pos)> mouseCallback = nullptr;
		std::function<void(const xVec2 &scroll)> scrollCallback = nullptr;
		std::function<void(unsigned int codepoint)> textCallback = nullptr;

	private:

		void *_context;
	};
}
