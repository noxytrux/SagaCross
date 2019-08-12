#include "SCMobileDisplay.h"

using namespace sc;

SCMobileDisplay::SCMobileDisplay(const uint32_t width, const uint32_t height, const bool fullscreen)
: SCDisplay(width, height, fullscreen)
{
}

SCMobileDisplay::~SCMobileDisplay() noexcept
{
}

void SCMobileDisplay::makeWindow()
{
}

const bool SCMobileDisplay::shouldClose() const
{
    return false;
}

void SCMobileDisplay::resizeWindow(const SCScreenSize & size)
{
    _size = size;
}

void SCMobileDisplay::resizeWindow(uint32_t width, uint32_t height)
{
    resizeWindow({ width, height });
}

const std::vector<SCScreenSize> SCMobileDisplay::getResolutions()
{
    return { _size };
}

void SCMobileDisplay::setFullScreen(bool fullscreen)
{
}

void SCMobileDisplay::setVsync(const bool vsync)
{
}
