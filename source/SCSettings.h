#pragma once

#include "Core-pch.hpp"

using json = nlohmann::json;

namespace sc {

	class SCSettings final : public SCNonCopyable {

	public:
		SCSettings(const std::string &path);
		~SCSettings();

		bool load();
		bool save();

		uint32_t getWidht() const { return _settings.width; }
		uint32_t getHeight() const { return _settings.height; }
		float getVolue() const { return _settings.volume; }
		bool isFullScreen() const { return _settings.fullscreen; }
		bool isVsyncEnabled() const { return _settings.vsync; }
		bool isMuted() const { return _settings.mute; }
		bool hardAIEnabled() const { return _settings.hardAI; }
		bool usesSecondCam() const { return _settings.secondCam; }

		void setWidht(const uint32_t width) { _settings.width = width; }
		void setHeight(const uint32_t height) { _settings.height = height; }
		void setVolume(const float volume) { _settings.volume = volume; }
		void setFullscreen(const bool fullscreen) { _settings.fullscreen = fullscreen; }
		void setVsyncEnabled(const bool vsync) { _settings.vsync = vsync; }
		void setMute(const bool mute) { _settings.mute = mute; }
		void setHardAIEnabled(const bool enabled) { _settings.hardAI = enabled; }
		void setUseSecondCam(const bool usecam) { _settings.secondCam = usecam; }

		//stores some application data shared across all objects 
		json userDefaults;

	private:

		struct settings {

			uint32_t width;
			uint32_t height;
			float volume;
			bool fullscreen;
			bool vsync;
			bool mute;
			bool hardAI;
			bool secondCam;
		};

		std::string _path;
		settings _settings;
	};

}

