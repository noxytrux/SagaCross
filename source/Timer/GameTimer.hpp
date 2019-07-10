//
//  GameTimer.hpp
//  PixFight
//
//  Created by Marcin Małysz on 17/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

#include <memory>

namespace sc {

	class GameTimer {

	public:

		GameTimer() {}
		virtual ~GameTimer() noexcept {}
		virtual void update() = 0;
		virtual double getElapsedSeconds() = 0;
	};

    extern std::shared_ptr<GameTimer> getPlatformTimerInstance();
}
