#pragma once

#include "SCRendererInterface.h"

namespace sc {

	class SCRenderable {

	public:
		SCRenderable() = default;

		virtual ~SCRenderable() noexcept = default;

		virtual void Render(const std::shared_ptr<SCRendererInterface> &renderer) = 0;
	};

}


