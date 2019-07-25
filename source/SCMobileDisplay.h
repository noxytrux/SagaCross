#pragma once

#include "Core-pch.hpp"
#include "SCDisplay.h"

namespace sc {

    class SCMobileDisplay final : public SCDisplay {
    public:

        SCMobileDisplay(const uint32_t width, const uint32_t height, const bool fullscreen);
        virtual ~SCMobileDisplay() noexcept;

        virtual void makeWindow() override;
        virtual const bool shouldClose() const override;

        virtual const bool allowMultipleResolutions() const override { return false; }

        virtual void resizeWindow(const SCScreenSize &size) override;
        virtual void resizeWindow(uint32_t width, uint32_t height) override;
        virtual const std::vector<SCScreenSize> getResolutions() override;
        virtual void setFullScreen(bool fullscreen) override;
        virtual void* getContext() const override { return nullptr; }
        virtual void setVsync(const bool vsync) override;
    };
}

