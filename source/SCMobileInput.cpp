#include "SCMobileInput.h"

using namespace sc;

SCMobileInput::SCMobileInput(void *context)
: SCInputInteface(context)
{
}

SCMobileInput::~SCMobileInput()
{
}

void SCMobileInput::update()
{
}

xVec2 SCMobileInput::getMousePosition()
{
    return xVec2(0, 0);
}

xVec2 SCMobileInput::getDoubleClickPosition()
{
    return xVec2(0, 0);
}

bool SCMobileInput::isLeftMouseButtonDown()
{
    return false;
}

bool SCMobileInput::isRightMouseButtonDown()
{
    return false;
}

bool SCMobileInput::isMiddleMouseButtonDown()
{
    return false;
}

bool SCMobileInput::isDoubleClickDown()
{
    return false;
}


