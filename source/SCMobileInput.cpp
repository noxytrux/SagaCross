#include "SCMobileInput.h"

using namespace sc;

SCMobileInput::SCMobileInput(void *context)
: SCInputInteface(context)
, mousePos(0)
, selected(false)
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
    return mousePos;
}

xVec2 SCMobileInput::getDoubleClickPosition()
{
    return mousePos;
}

bool SCMobileInput::isLeftMouseButtonDown()
{
    bool state = selected;
    selected = false;
    return state;
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


