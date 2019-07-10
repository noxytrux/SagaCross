//
//  GameTimer.cpp
//  PixFight
//
//  Created by Marcin Małysz on 17/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#include "GameTimer.hpp"

using namespace sc;

#ifdef __linux__
    #include "AndroidTimer.hpp"
#endif

#ifdef _WIN32
	#include "WindowsTimer.hpp"
#endif

#ifdef __APPLE__
    #include "DarwinTimer.hpp"
#endif

#ifdef __ANDROID__
    #include "AndroidTimer.hpp"
#endif

#ifdef __EMSCRIPTEN__
    #include "AndroidTimer.hpp"
#endif

std::shared_ptr<GameTimer> sc::getPlatformTimerInstance() {

#if defined(__linux__) || defined(__ANDROID__) || defined(__EMSCRIPTEN__)
    return std::make_shared<AndroidTimer>();
#endif

#ifdef _WIN32
	return std::make_shared<WindowsTimer>();
#endif

#ifdef __APPLE__
    return std::make_shared<DarwinTimer>();
#endif

    return nullptr;
}
