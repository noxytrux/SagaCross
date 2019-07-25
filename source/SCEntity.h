#pragma once

#include <memory>

#include "SCNonCopyable.hpp"

namespace sc {

	class SCEntity : public SCNonCopyable, public std::enable_shared_from_this<SCEntity> {
	public:
		
		SCEntity();
		virtual ~SCEntity();
	
		virtual void Think(const float &deltaTime) = 0;
	};
}
