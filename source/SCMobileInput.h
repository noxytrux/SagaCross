#pragma once

#include "SCInputInterface.h"

namespace sc {

    class SCMobileInput : public SCInputInteface
    {
    public:

        SCMobileInput(void *context);
        virtual ~SCMobileInput();

        virtual void update() override;

        virtual xVec2 getMousePosition() override;
        virtual xVec2 getDoubleClickPosition() override;

        virtual bool isLeftMouseButtonDown() override;
        virtual bool isRightMouseButtonDown() override;
        virtual bool isMiddleMouseButtonDown() override;
        virtual bool isDoubleClickDown() override;

    public:

        xVec2 mousePos;
        bool selected;
    };
}


