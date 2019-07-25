#include "SCDisplay.h"

using namespace sc;

SCDisplay::SCDisplay(const uint32_t width, const uint32_t height, const bool fullscreen)
	: _size({width, height})
	, _fullscreen(fullscreen)
{ 
}

SCDisplay::~SCDisplay() noexcept
{ 
}

